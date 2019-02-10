//av_file_source.cpp

#include "av_file_source.h"
#include "codec_specify.h"

AVParam* AvFileSource::get()
{
	AVPacket *pack = av_packet_alloc();
	av_init_packet(pack);
	bool isok = false;
	if (readp(pack) == AV_READ_OK)
	{
		_param->setData(pack->data, pack->size);
		_param->pts = pack->pts;
		_param->dts = pack->dts;
		_param->codecid = _ffmpeg2codec(_vcodecid);
		_param->w = width();
		_param->h = height();
		_param->framerate = framerate();
        _param->format = _format_ctx->streams[_videostream]->codecpar->format;
		isok =true;
	}

	av_packet_free(&pack);
	if (isok)
		return _param;

	return nullptr;
}

void AvFileSource::open(const std::string& filename)throw(AvException)
{
	AVFormatContext *formatCtx = NULL;
	int ret = -1;
	if ((ret = avformat_open_input(&_format_ctx, filename.c_str(), NULL, NULL)) < 0)
		throw AvException(ret, __FILE__, __LINE__);

	initParams();
}

void AvFileSource::close()
{
	avformat_free_context(_format_ctx);
	_format_ctx = NULL;
}

AvFileSource::AvReadRet AvFileSource::readp(AVPacket* packet)
{
	int ret = -1;
	do {
		ret = av_read_frame(_format_ctx, packet);
		if (ret != 0)
			return AV_EOF;

	} while (packet->stream_index != _videostream);

	return AV_READ_OK;
}

void AvFileSource::initParams() throw(AvException)
{
    int ret = -1;
    if ((ret = avformat_find_stream_info(_format_ctx, NULL))< 0)
        throw AvException(ret, __FILE__, __LINE__);
        
        for(uint32_t i=0; i<_format_ctx->nb_streams; i++)
        {
            if( _format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
            {
                _videostream = i;
                _vcodecid = _format_ctx->streams[_videostream]->codecpar->codec_id;
                _width = _format_ctx->streams[_videostream]->codecpar->width;
                _height = _format_ctx->streams[_videostream]->codecpar->height;
                _framerate = _format_ctx->streams[_videostream]->r_frame_rate.num;
		_codec_info.codecpar = _format_ctx->streams[_videostream]->codecpar;
            }
            else if(_format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
            {
                _audiostream = i;
                _acodecid = _format_ctx->streams[_audiostream]->codecpar->codec_id;
            }
        }
}
