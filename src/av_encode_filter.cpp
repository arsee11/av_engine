
///  av_encode_filter.cpp
#include "av_encode_filter.h"

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/error.h>
    
}

#include "codec_specify.h"
#include "av_log.h"

bool AvEncodeFilter::transform(AVParam*& p)
{
	if (_codec_ctx == nullptr)
	{
		if (!open((PixelFormat)p->format, p->w, p->h, _framerate))
			return false;
	}
		   
	p->framerate = _framerate;

	bool isok = false;
    AVPixelFormat f = _2ffmpeg_format((PixelFormat)p->format);
    AVFrame *frame = av_frame_alloc();	
    av_image_fill_arrays(frame->data, frame->linesize, p->getData(), f, p->w, p->h, 1);
    frame->width = p->w;
    frame->height= p->h;
    frame->format = f;
	frame->pts = _frame_count++;
    int rc = avcodec_send_frame(_codec_ctx, frame);
	if (rc == 0)
	{
		AVPacket* pack = av_packet_alloc();
		rc = avcodec_receive_packet(_codec_ctx, pack);
		if (rc == 0)
		{
			p->setData(pack->data, pack->size);
			p->pts = pack->pts;
			p->dts = pack->pts;
			p->framerate = _framerate;
			isok = true;
		}
		av_packet_free(&pack);
	}
          
    
    av_frame_unref(frame);
    return isok;
    
}

bool AvEncodeFilter::open(PixelFormat f, int width, int height, int framerate)
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
    
    _codec_ctx->bit_rate = width*height*3 * 1024;
    _codec_ctx->width =width;
    _codec_ctx->height = height;
    AVRational rate;
    rate.num = 1;
    rate.den = framerate;
    _codec_ctx->time_base= rate;
    _codec_ctx->gop_size = 8;
    _codec_ctx->max_b_frames=1;
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
	return true;
}

