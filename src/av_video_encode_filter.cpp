
///  av_video_encode_filter.cpp
#include "av_video_encode_filter.h"

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/error.h>
    
}

#include "codec_specify.h"
#include "av_log.h"

bool AvVideoEncodeFilter::transform(AVParam* p)
{
	if (_codec_ctx == nullptr)
	{
		if (!open((PixelFormat)p->format, p->w, p->h, _framerate))
			return false;
	}
		   
	bool isok = false;
	AVPixelFormat f = _2ffmpeg_format((PixelFormat)p->format);
	AVFrame *frame = av_frame_alloc();	
	av_image_fill_arrays(frame->data, frame->linesize, p->data_ptr(), f, p->w, p->h, 1);
	frame->width = p->w;
	frame->height= p->h;
	frame->format = f;
	frame->pts = _frame_count++;
	int rc = avcodec_send_frame(_codec_ctx, frame);
	if (rc == 0)
	{
		rc = avcodec_receive_packet(_codec_ctx, _pack);
		if (rc == 0)
		{
			//av_log_info()<<"encoder frame size="<<pack->size<<end_log();
			//av_log_info()<<"encoder frame pts="<<pack->pts<<",dts="<<pack->dts<<end_log();
			_param.data(_pack->data, _pack->size);
			_param.pts = _pack->pts;
			isok = true;
			av_packet_unref(_pack);
		}
	}
          
    av_frame_unref(frame);
	return isok;
    
}

bool AvVideoEncodeFilter::open(PixelFormat f, int width, int height, int framerate)
{
    AVCodec* codec = avcodec_find_encoder(  _2ffmpeg_id(_codec_id) );
    if(codec == NULL )
    {
        av_log_error()<<"encoder[id="<<_codec_id<<"] not found"<<end_log();
        return false;
    }
    
    _codec_ctx = avcodec_alloc_context3(codec);
    if(_codec_ctx == NULL)
        return false;
    
    _codec_ctx->bit_rate = _bitrate;
    _codec_ctx->width =width;
    _codec_ctx->height = height;
    AVRational rate;
    rate.num = 1;
    rate.den = framerate;
    _codec_ctx->time_base= rate;
    _codec_ctx->gop_size = _gop; 
    _codec_ctx->max_b_frames=0;
    _codec_ctx->thread_count = 4;
    _codec_ctx->pix_fmt = _2ffmpeg_format(f);
    AVDictionary* opts=NULL;
    av_dict_set(&opts, "allow_sw", "1", 0); //allows software encoding
    int ret=0;
    if( (ret=avcodec_open2(_codec_ctx, codec, &opts)) < 0)
    {
        char buf[256];
        av_make_error_string(buf, 256, ret);
        av_log_error()<<"encoder avcodec_open2 failed:"<<buf<<end_log();
		avcodec_close(_codec_ctx);
		_codec_ctx = nullptr;
        return false;
    }
    
	_framerate = framerate;
	_param.fps = _framerate;
	_param.w= width;
	_param.h= height;
	_param.codecid = _codec_id;
	_param.type = MEDIA_VIDEO;
	_pack = av_packet_alloc();

	return true;
}

void AvVideoEncodeFilter::close()
{
	if (_codec_ctx != nullptr)
	{
		avcodec_free_context(&_codec_ctx);
	}
	if (_pack != nullptr) {
		av_packet_free(&_pack);
	}
}
