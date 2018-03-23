///frame_scaler.h

#ifndef FRAME_SCALER_H
#define FRAME_SCALER_H

extern "C"{
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

#include <new>
#include "av_util.h"
#include <tuple>

class FrameScaler
{
public:
	FrameScaler(int srcWith, int srcHeight, PixelFormat src_pix_fmt, int dstWith, int dstHeight, PixelFormat dst_pix_fmt)
		:_srcWith			(srcWith)
		, _srcHeight		(srcHeight)
		, _src_pix_fmt	(src_pix_fmt)
		,_dstWith			(dstWith)
		,_dstHeight		(dstHeight)
		,_dst_pix_fmt	(dst_pix_fmt)
		,_dst_frame_data(NULL)
	{
		open(srcWith, srcHeight, src_pix_fmt, dstWith, dstHeight, dst_pix_fmt);
	}
    
    ~FrameScaler(){
        av_frame_free(&_destFrame);
        av_frame_free(&_srcFrame);
        sws_freeContext(_sws_ctx);
		free(_dst_frame_data);
    }

    void scale(const uint8_t* data, int len)
	{
        
        av_image_fill_arrays(_srcFrame->data, _srcFrame->linesize, data, _2ffmpeg_format(_src_pix_fmt), _srcWith, _srcHeight, 1);
        int dst_height = sws_scale(
                            _sws_ctx,
                            _srcFrame->data,
                            _srcFrame->linesize,
                            0,
                            _srcHeight,
                            _destFrame->data,
                            _destFrame->linesize
                  );

		_destFrame->width = _dstWith;
        _destFrame->height = dst_height;
        _destFrame->format = _2ffmpeg_format(_dst_pix_fmt);
        av_image_copy_to_buffer(_dst_frame_data
                                ,_dst_frame_size
                                ,_destFrame->data
                                ,_destFrame->linesize
                                ,_2ffmpeg_format(_dst_pix_fmt)
                                ,_dstWith, _dstHeight, 1);

	}

    int dst_frame_size(){ return _dst_frame_size;}
	uint8_t* dst_frame_data() { return _dst_frame_data; }

private:
    void open(int srcWith, int srcHeight, PixelFormat src_pix_fmt, int dstWith, int dstHeight, PixelFormat dst_pix_fmt)
	{
        _srcFrame = av_frame_alloc();
        _destFrame = av_frame_alloc();
        _sws_ctx = sws_getContext(srcWith, srcHeight, _2ffmpeg_format(src_pix_fmt),
				dstWith, dstHeight, _2ffmpeg_format(dst_pix_fmt),
				SWS_BILINEAR, NULL, 	NULL, NULL
		);
        
		_dst_frame_size = av_image_get_buffer_size(_2ffmpeg_format(dst_pix_fmt)
            ,dstWith ,dstHeight ,1
		);
        
		_dst_frame_data = (uint8_t *)malloc( _dst_frame_size );
        if(_dst_frame_data == NULL)
        {
            throw std::bad_alloc();
        }

		av_image_fill_arrays(_destFrame->data, _destFrame->linesize, _dst_frame_data, _2ffmpeg_format(dst_pix_fmt),
            dstWith, dstHeight, 1
		);
        
    }
	
private:
	AVFrame* _destFrame, *_srcFrame;
	struct SwsContext* _sws_ctx;
	int _srcWith;
	int _srcHeight;
	PixelFormat _src_pix_fmt;
	int _dstWith;
	int _dstHeight;
	PixelFormat _dst_pix_fmt;
    int _dst_frame_size=0;
	uint8_t* _dst_frame_data;
};

#endif /*FRAME_SCALER_H*/
