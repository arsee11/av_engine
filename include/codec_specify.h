///codec_specify.h

#ifndef CODE_SPECIFY_H
#define CODE_SPECIFY_H

#include <libavcodec/avcodec.h>

enum CodecID
{
	/*video*/
	CODEC_ID_NONE,
    H264,
	MPEG4,
	MSMPEG4V3,

	/*audio*/
	AUDIO_RAW=1000,
	MP3,
	AAC,
	G729,
	WMAV2,
};

enum PixelFormat
{
    FORMAT_NONE=-1,
    FORMAT_RGB24,
    FORMAT_YUV420,
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

AVCodecID _2ffmpeg_id(CodecID cid);
CodecID _ffmpeg2codec(AVCodecID cid);
AVPixelFormat _2ffmpeg_format(PixelFormat f);
PixelFormat ffmpeg2format(AVPixelFormat f);
AVSampleFormat _2ffmpeg_format(SampleFormat f);

#endif /*CODE_SPECIFY_H*/
