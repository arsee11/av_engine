
//  av_camera.cpp


#include "av_camera.h"
#include <stdio.h>
#include <iostream>
#include "codec_specify.h"
#include "av_log.h"

using namespace std;


extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
}

#include "codec_specify.h"

AvCamera::AvCamera(Transformation<Param>* ts)
	:Source(ts)
{}

AvCamera::AvCamera(int framerate, int width, int height, Transformation<Param>* ts)
	:Source(ts)
{
	_param.fps = framerate;
	_param.w = width;
	_param.h = height;
}

AvCamera::AvCamera(int framerate, int width, int height)
	:AvCamera(framerate, width, height, nullptr)
{
	
}

bool AvCamera::open(const char* device)
{
	return open(device, _param.fps, _param.w, _param.h);
}

bool AvCamera::open(const char* device, int framerate, int width, int height)
{
	if (_is_opened)
		return true;

#ifdef WIN32
    const AVInputFormat *ifmt=av_find_input_format("dshow");
    AVDictionary* options = NULL;
#endif

#ifdef LINUX 
    const AVInputFormat *ifmt=av_find_input_format("v4l2");
    AVDictionary* options = NULL;
#endif

#ifdef MACOS
    const AVInputFormat *ifmt=av_find_input_format("avfoundation");
    AVDictionary* options = NULL;
#endif

    char optstr[32]={0};
    snprintf(optstr, 31, "%dx%d", width, height);
    av_dict_set(&options, "video_size", optstr, 0);
    av_dict_set_int(&options,"framerate", framerate, 0);

    int ec;
    if((ec=avformat_open_input(&_format_ctx, device, ifmt, &options))!=0)
    {
        char buf[128];
		av_make_error_string(buf, 128, ec);
		av_log_output(LOGL_ERROR, (std::string("Couldn't open AvCamera. error:") + std::string(buf)).c_str());
        avformat_free_context(_format_ctx);
        return false;
    }


    if( (ec=avformat_find_stream_info(_format_ctx, NULL)) < 0 )
    {
        char buf[128];
		av_make_error_string(buf, 128, ec);
        av_log_output(LOGL_ERROR, (std::string("Couldn't open AvCamera. error:")+std::string(buf)).c_str());
		avformat_free_context(_format_ctx);
        return false; // Couldn't find stream information
    }

    for(uint32_t i=0; i<_format_ctx->nb_streams; i++)
    {
        if(_format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            _video_stream_idx = i;
            break;
        }
    }

    _param.codecid = _ffmpeg2codec(_format_ctx->streams[_video_stream_idx]->codecpar->codec_id);
    _param.w = width;
    _param.h = height;
    _param.format = ffmpeg2format( (AVPixelFormat)_format_ctx->streams[_video_stream_idx]->codecpar->format);
    _param.fps = framerate;
    _param.type = MEDIA_VIDEO;
    _pack = av_packet_alloc();

    return true;
}

AVParam* AvCamera::get()
{
	assert(_format_ctx);
	_param.clear();
	while (true)
	{
		av_packet_unref(_pack);
		if (av_read_frame(_format_ctx, _pack) >= 0)
		{
			if (_pack->stream_index == _video_stream_idx)
			{
                _param.data(_pack->data, _pack->size);
                break;
            }
		}

	}
	return &_param;
}

void AvCamera::close()
{
    avformat_close_input(&_format_ctx);
    if (_pack != nullptr) {
        av_packet_free(&_pack);
    }
    _is_opened = false;

}

CodecInfo AvCamera::codec_info()
{
	CodecInfo ci; 
	ci.codec = _param.codecid;
	ci.w = _param.w;
	ci.h = _param.h;
	ci.pix_format= static_cast<PixelFormat>(_param.format);
	ci.fps = _param.fps;
	return ci; 
}
