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
	CODEC_ID_NONE,
    	H264,
	MPEG4,
	MSMPEG4V3,

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
    FORMAT_YUV420,
    FORMAT_YUV422,
    FORMAT_YUYV422,
    FORMAT_UYVY422
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
	AVCodecParameters *codecpar;
};

AVCodecID _2ffmpeg_id(CodecID cid);
CodecID _ffmpeg2codec(AVCodecID cid);
AVPixelFormat _2ffmpeg_format(PixelFormat f);
PixelFormat ffmpeg2format(AVPixelFormat f);
AVSampleFormat _2ffmpeg_format(SampleFormat f);
SampleFormat ffmpeg2format(AVSampleFormat f);

#endif /*CODE_SPECIFY_H*/
