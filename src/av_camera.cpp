
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
{
	
}

bool AvCamera::open(const char* device, int framerate, int width, int height)
{
	if (_is_opened)
		return true;

#ifdef WIN32
    AVInputFormat *ifmt=av_find_input_format("dshow");
    AVDictionary* options = NULL;
#endif

#ifdef LINUX 
    AVInputFormat *ifmt=av_find_input_format("v4l2");
    AVDictionary* options = NULL;
#endif

#ifdef MACOS
    AVInputFormat *ifmt=av_find_input_format("avfoundation");
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

    _width = width;
    _height = height;
    _framerate = framerate;

    _param.w = _width;
    _param.h = _height;
    _param.format = ffmpeg2format( (AVPixelFormat)_format_ctx->streams[_video_stream_idx]->codecpar->format);
    _param.type = MEDIA_VIDEO;

    return true;
}

AVParam* AvCamera::get()
{
	assert(_format_ctx);
    	AVPacket* packet = av_packet_alloc();
	_param.clear();
	while (true)
	{
		if (av_read_frame(_format_ctx, packet) >= 0)
		{
			if (packet->stream_index == _video_stream_idx)
			{
            	    		_param.data(packet->data, packet->size);
                		break;
            		}
		}

		av_packet_unref(packet);
	}
	av_packet_free(&packet);
	return &_param;
}

void AvCamera::close()
{
    avformat_close_input(&_format_ctx);
    _is_opened = false;
}
