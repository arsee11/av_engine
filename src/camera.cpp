
//  camera.cpp


#include "camera.h"
#include <stdio.h>
#include <iostream>
#include "codec_specify.h"

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

Camera::Camera(Transformation<Param>* ts)
	:Source(ts)
{
	
}

bool Camera::open(const char* device, int framerate, int width, int height)
{
	if (_is_opened)
		return true;

#ifdef WIN32
    AVInputFormat *ifmt=av_find_input_format("dshow");
    AVDictionary* options = NULL;

    char optstr[32]={0};
    sprintf(optstr, "%dx%d", width, height);
    av_dict_set(&options, "video_size", optstr, 0);
    av_dict_set_int(&options,"framerate", framerate, 0);
#else
    AVInputFormat *ifmt=av_find_input_format("avfoundation");
    AVDictionary* options = NULL;
    
    char optstr[32]={0};
    sprintf(optstr, "%dx%d", width, height);
    av_dict_set(&options, "video_size", optstr, 0);
    sprintf(optstr, "%d", framerate);
    av_dict_set(&options,"framerate",optstr,0);
#endif

    int ec;
    if((ec=avformat_open_input(&_format_ctx, device, ifmt, &options))!=0)
    {
        char buf[128];
        cout<<"Couldn't open camera. error:"<<av_make_error_string(buf, 128, ec)<<endl;
        avformat_free_context(_format_ctx);
        return false;
    }


    if( (ec=avformat_find_stream_info(_format_ctx, NULL)) < 0 )
    {
        char buf[128];
        printf("Couldn't open camera. error:%s\n", av_make_error_string(buf, 128, ec));
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
    //AVRational time_base = {1, framerate};  

    return true;
}

bool Camera::transform(AVParam*& p)
{
	assert(_format_ctx);
    AVPacket* packet = av_packet_alloc();
	while (true)
	{
		if (av_read_frame(_format_ctx, packet) >= 0)
		{
			if (packet->stream_index == _video_stream_idx)
			{
                _param->setData(packet->data, packet->size);
                _param->w = _width;
                _param->h = _height;
                _param->format = ffmpeg2format( (AVPixelFormat)_format_ctx->streams[_video_stream_idx]->codecpar->format);
				_param->type = MEDIA_VIDEO;
                break;
            }
		}

		av_packet_unref(packet);
	}
	av_packet_free(&packet);

	p = _param;
	return true;
}

void Camera::close()
{
    avformat_close_input(&_format_ctx);
    _is_opened = false;
}
