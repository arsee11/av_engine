///codec_specify.h

#ifndef CODE_SPECIFY_H
#define CODE_SPECIFY_H

extern "C"
{
#include <libavcodec/avcodec.h>
}

enum CodecID
{
    /*video*/
    CODEC_ID_NONE, //raw video
    H264,
    MPEG4,
    MSMPEG4V3,
    MJPEG,
    H265,
    VP9,
    AV1,

    /*audio*/
    S16LE=1000,
    S16BE,
    MP3,
    AAC,
    G729,
    WMAV2,
    PCMU,
    PCMA
};

enum PixelFormat
{
    FORMAT_NONE=-1,
    FORMAT_RGB24,
    FORMAT_ARGB,
    FORMAT_YUV420P,
    FORMAT_YUV422P,
    FORMAT_YUV444P,
    FORMAT_NV12
};

enum SampleFormat
{
    NONE = -1,
    U8,
    S16,
    FLT,
    U8P,
    S16P,
    FLTP
};

enum MediaType
{
    MEDIA_NONE = -1,
    MEDIA_AUDIO,
    MEDIA_VIDEO
};

struct CodecInfo
{
    CodecID codec;
    MediaType codec_type;
    void* codecpar;
    union {
        struct {
            int w;  //video frame width
            int h;	//video frame height
            int fps;//video frame rate
            PixelFormat pix_format;
        };
        struct {
            int sr; //audio smaple_rate
            int nchn;//number of audio channels.
            int nsamples;//mumber of smaples per channel.
            SampleFormat sp_format;
        };
    };
};

AVCodecID _2ffmpeg_id(CodecID cid);
CodecID _ffmpeg2codec(AVCodecID cid);
AVPixelFormat _2ffmpeg_format(PixelFormat f);
PixelFormat ffmpeg2format(AVPixelFormat f);
AVSampleFormat _2ffmpeg_format(SampleFormat f);
SampleFormat ffmpeg2format(AVSampleFormat f);
int sample_size(SampleFormat f);

#endif /*CODE_SPECIFY_H*/
