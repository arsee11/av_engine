///frame_scaler.cpp

#include "frame_scaler.h"
#include <fstream>

#define _DUMP 0

#if _DUMP

static void dump_frame(uint8_t* frame_data, size_t frame_size)
{
    static std::ofstream fpOut("out.scale", std::ios::out | std::ios::binary);

    fpOut.write(reinterpret_cast<char*>(frame_data), frame_size);

   
}
#endif 

void FrameScaler::scale(const uint8_t* data, int len)
{
    if (data == nullptr || len == 0) {
        return;
    }
    av_image_fill_arrays(_srcFrame->data, _srcFrame->linesize, data,
        convertDeprecatedFormat(_2ffmpeg_format(_src_pix_fmt)),
        _srcWith, _srcHeight, 1
    );
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

#if _DUMP
    dump_frame(_dst_frame_data, _dst_frame_size);
#endif

}

void FrameScaler::open(int srcWith, int srcHeight, PixelFormat src_pix_fmt
    ,int dstWith, int dstHeight, PixelFormat dst_pix_fmt
)
{
    _srcFrame = av_frame_alloc();
    _destFrame = av_frame_alloc();
    _sws_ctx = sws_getContext(srcWith, srcHeight,
    convertDeprecatedFormat(_2ffmpeg_format(src_pix_fmt)),
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

    av_image_fill_arrays(_destFrame->data, _destFrame->linesize, _dst_frame_data,
        _2ffmpeg_format(dst_pix_fmt),
        dstWith, dstHeight, 1
    );
    
}
AVPixelFormat FrameScaler::convertDeprecatedFormat(AVPixelFormat format)
{
    switch (format)
    {
    case AV_PIX_FMT_YUVJ420P:
        return AV_PIX_FMT_YUV420P;
        break;
    case AV_PIX_FMT_YUVJ422P:
        return AV_PIX_FMT_YUV422P;
        break;
    case AV_PIX_FMT_YUVJ444P:
        return AV_PIX_FMT_YUV444P;
        break;
    case AV_PIX_FMT_YUVJ440P:
        return AV_PIX_FMT_YUV440P;
        break;
    default:
        return format;
        break;
    }
}

