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
    FrameScaler(int srcWith, int srcHeight, PixelFormat src_pix_fmt
        ,int dstWith, int dstHeight, PixelFormat dst_pix_fmt
    )
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

    void scale(const uint8_t* data, int len);
    int dst_frame_size(){ return _dst_frame_size;}
    uint8_t* dst_frame_data() { return _dst_frame_data; }

private:
    void open(int srcWith, int srcHeight, PixelFormat src_pix_fmt
        ,int dstWith, int dstHeight, PixelFormat dst_pix_fmt
    );
    AVPixelFormat convertDeprecatedFormat(AVPixelFormat format);

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
