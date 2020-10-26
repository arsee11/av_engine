//av_file_source.cpp

#include "av_file_source.h"
#include "codec_specify.h"

AVParam* AvFileSource::get()
{
	bool isok=false;
	av_packet_unref(_pack);
	if (readp(_pack) == AV_READ_OK)
	{

	 	if(_pack->stream_index == _videostream)
		{
			_param.data(_pack->data, _pack->size);
			_param.type = MediaType::MEDIA_VIDEO;
			_param.pts = _pack->pts;
			_param.codecid = _codec_infos[MediaType::MEDIA_VIDEO].codec;
			_param.w = _codec_infos[MediaType::MEDIA_VIDEO].w;
			_param.h = _codec_infos[MediaType::MEDIA_VIDEO].h;
			_param.fps = _codec_infos[MediaType::MEDIA_VIDEO].fps;
    		_param.format = _codec_infos[MediaType::MEDIA_VIDEO].pix_format;
			isok=true;
		}
		else if(_pack->stream_index == _audiostream)
		{
			_param.data(_pack->data, _pack->size);
			_param.type = MediaType::MEDIA_AUDIO;
			_param.codecid = _codec_infos[MediaType::MEDIA_AUDIO].codec;
			_param.format = _codec_infos[MediaType::MEDIA_AUDIO].sp_format;
			_param.sr = _codec_infos[MediaType::MEDIA_AUDIO].sr;
			_param.nchn = _codec_infos[MediaType::MEDIA_AUDIO].nchn;
			_param.nsamples = _codec_infos[MediaType::MEDIA_AUDIO].nsamples;

			isok=true;
		}
	}

	if(isok)
		return &_param;

	return nullptr;
}

void AvFileSource::open(const std::string& filename)
{
	AVFormatContext *formatCtx = nullptr;
	int ret = -1;
	if ((ret = avformat_open_input(&_format_ctx, filename.c_str(), nullptr, nullptr)) < 0)
		throw AvException(ret, __FILE__, __LINE__);

	initParams();
	_pack = av_packet_alloc();
}

void AvFileSource::close()
{
	if(_format_ctx != nullptr)
    {   
        avformat_close_input(&_format_ctx);
	    avformat_free_context(_format_ctx);
	    _format_ctx = nullptr;
    }
	if (_pack != nullptr) {
		av_packet_free(&_pack);
		_pack = nullptr;
	}
}

AvFileSource::AvReadRet AvFileSource::readp(AVPacket* packet)
{
	int ret = -1;

	ret = av_read_frame(_format_ctx, packet);
	if (ret != 0)
		return AV_EOF;

	return AV_READ_OK;
}

void AvFileSource::initParams()
{
    int ret = -1;
    if ((ret = avformat_find_stream_info(_format_ctx, nullptr))< 0)
        throw AvException(ret, __FILE__, __LINE__);
        
        for(uint32_t i=0; i<_format_ctx->nb_streams; i++)
        {
			AVCodecParameters* par = _format_ctx->streams[i]->codecpar;
            if( _format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
            {
                _videostream = i;
				CodecInfo ci;
				ci.codec_type = MediaType::MEDIA_VIDEO;
				ci.codec = _ffmpeg2codec(par->codec_id);
				ci.pix_format = ffmpeg2format((AVPixelFormat)par->format);
				ci.w = par->width;
				ci.h = par->height;
				ci.fps = _format_ctx->streams[i]->r_frame_rate.num/_format_ctx->streams[i]->r_frame_rate.den;
				ci.codecpar = par;
				_codec_infos.insert({ ci.codec_type, ci });
            }
            else if(_format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
            {
                _audiostream = i;
				CodecInfo ci;
				ci.codec_type = MediaType::MEDIA_AUDIO;
				ci.codec = _ffmpeg2codec(par->codec_id);
				ci.sp_format = ffmpeg2format((AVSampleFormat)par->format);
				ci.sr = par->sample_rate;
				ci.nchn = par->channels;
				ci.nsamples = par->frame_size;
				_codec_infos.insert({ ci.codec_type, ci });
            }
        }
}
