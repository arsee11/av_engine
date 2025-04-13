//av_file_source.cpp

#include "av_file_source.h"
#include "codec_specify.h"
#include "av_log.h"

AVParam* AvFileSource::get()
{
	bool isok=false;
	av_packet_unref(_pack);
	if (readp(_pack) == AV_READ_OK)
	{

	 	if(_pack->stream_index == _videostream)
		{
			_param.codecid = _codec_infos[MediaType::MEDIA_VIDEO].codec;
			_param.type = MediaType::MEDIA_VIDEO;
			_param.w = _codec_infos[MediaType::MEDIA_VIDEO].w;
			_param.h = _codec_infos[MediaType::MEDIA_VIDEO].h;
			_param.fps = _codec_infos[MediaType::MEDIA_VIDEO].fps;
			_param.format = _codec_infos[MediaType::MEDIA_VIDEO].pix_format;
			if (_isMp4H264 || _isMp4H265) {
				if (_pack_read->data) {
					av_packet_unref(_pack_read);
				}
				if (av_bsf_send_packet(_bsfc, _pack) < 0) {
					av_log_error() << "av_bsf_send_packet failed";
					return nullptr;
				}
			
				if (av_bsf_receive_packet(_bsfc, _pack_read) < 0) {
					av_log_error() << "av_bsf_receive_packet failed";
					return nullptr;
				}
				_param.data(_pack_read->data, _pack_read->size);
				_param.pts = _pack_read->pts;
			}
			else {
				_param.data(_pack->data, _pack->size);				
				_param.pts = _pack->pts;
			}
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
	_pack_read = av_packet_alloc();
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

	if (_pack_read != nullptr) {
		av_packet_free(&_pack_read);
		_pack_read = nullptr;
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

				_isMp4H264 = par->codec_id == AV_CODEC_ID_H264 && (
					!strcmp(_format_ctx->iformat->long_name, "QuickTime / MOV")
					|| !strcmp(_format_ctx->iformat->long_name, "FLV (Flash Video)")
					|| !strcmp(_format_ctx->iformat->long_name, "Matroska / WebM")
					);
				_isMp4H265 = par->codec_id == AV_CODEC_ID_HEVC && (
					!strcmp(_format_ctx->iformat->long_name, "QuickTime / MOV")
					|| !strcmp(_format_ctx->iformat->long_name, "FLV (Flash Video)")
					|| !strcmp(_format_ctx->iformat->long_name, "Matroska / WebM")
					);

				// Initialize bitstream filter and its required resources
				if (_isMp4H264) {
					const AVBitStreamFilter* bsf = av_bsf_get_by_name("h264_mp4toannexb");
					if (!bsf) {
						av_log_error() << "FFmpeg error: " << __FILE__ << " " << __LINE__ << " " << "av_bsf_get_by_name() failed";
						return;
					}
					av_bsf_alloc(bsf, &_bsfc);
					avcodec_parameters_copy(_bsfc->par_in, _format_ctx->streams[i]->codecpar);
					av_bsf_init(_bsfc);
				}
				if (_isMp4H265) {
					const AVBitStreamFilter* bsf = av_bsf_get_by_name("hevc_mp4toannexb");
					if (!bsf) {
						av_log_error() << "FFmpeg error: " << __FILE__ << " " << __LINE__ << " " << "av_bsf_get_by_name() failed";
						return;
					}
					av_bsf_alloc(bsf, &_bsfc);
					avcodec_parameters_copy(_bsfc->par_in, _format_ctx->streams[i]->codecpar);
					av_bsf_init(_bsfc);
				}

				CodecInfo ci;
				ci.codec_type = MediaType::MEDIA_VIDEO;
				ci.codec = _ffmpeg2codec(par->codec_id);
				ci.pix_format = ffmpeg2format((AVPixelFormat)par->format);
				ci.w = par->width;
				ci.h = par->height;
				ci.fps = _format_ctx->streams[i]->r_frame_rate.num / _format_ctx->streams[i]->r_frame_rate.den;
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
				ci.nchn = par->ch_layout.nb_channels;
				ci.nsamples = par->frame_size;
				_codec_infos.insert({ ci.codec_type, ci });
            }
        }
}
