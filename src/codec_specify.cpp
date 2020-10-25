//codec_specify.cpp


#include "codec_specify.h"

PixelFormat ffmpeg2format(AVPixelFormat f)
{
    switch (f) {
        case AV_PIX_FMT_RGB24: return FORMAT_RGB24;
        case AV_PIX_FMT_YUV420P: return FORMAT_YUV420;
        case AV_PIX_FMT_YUV422P: return FORMAT_YUV422;
        case AV_PIX_FMT_YUV444P: return FORMAT_YUV444;
        case AV_PIX_FMT_YUYV422: return FORMAT_YUYV422;
        case AV_PIX_FMT_UYVY422: return FORMAT_UYVY422;
        case AV_PIX_FMT_YUVJ420P: return FORMAT_YUVJ420;
        case AV_PIX_FMT_YUVJ422P: return FORMAT_YUVJ422;
        case AV_PIX_FMT_YUVJ444P: return FORMAT_YUVJ444;

        default:
            return FORMAT_NONE;
    }
}

AVCodecID _2ffmpeg_id(CodecID cid)
{
    switch(cid)
    {
        case H264: return AV_CODEC_ID_H264;
		case MSMPEG4V3: return AV_CODEC_ID_MSMPEG4V3;
		case MPEG4: return AV_CODEC_ID_MPEG4;
		case S16LE: return AV_CODEC_ID_PCM_S16LE;
		case MP3: return AV_CODEC_ID_MP3;
		case AAC: return AV_CODEC_ID_AAC;
		case G729: return AV_CODEC_ID_G729;
		case WMAV2: return AV_CODEC_ID_WMAV2;
		case PCMA: return AV_CODEC_ID_PCM_ALAW;
		case PCMU: return AV_CODEC_ID_PCM_MULAW;
		case MJPEG: return AV_CODEC_ID_MJPEG;

        default: return AV_CODEC_ID_NONE;
    }
}

CodecID _ffmpeg2codec(AVCodecID cid)
{
	switch (cid)
	{
	case AV_CODEC_ID_H264 : return H264;
	case AV_CODEC_ID_MSMPEG4V3: return MSMPEG4V3;
	case AV_CODEC_ID_MPEG4: return MPEG4;
	case AV_CODEC_ID_PCM_S16LE: return S16LE;
	case AV_CODEC_ID_MP3: return MP3;
	case AV_CODEC_ID_AAC: return AAC;
	case AV_CODEC_ID_G729: return G729;
	case AV_CODEC_ID_WMAV2: return WMAV2;
	case AV_CODEC_ID_PCM_ALAW: return PCMA;
	case AV_CODEC_ID_PCM_MULAW: return PCMU;
	case AV_CODEC_ID_MJPEG: return MJPEG;

	default: return CODEC_ID_NONE;
	}
}

AVPixelFormat _2ffmpeg_format(PixelFormat f)
{
    switch (f)
	{
        case FORMAT_RGB24: return AV_PIX_FMT_RGB24;
        case FORMAT_YUV420: return AV_PIX_FMT_YUV420P;
        case FORMAT_YUV422: return AV_PIX_FMT_YUV422P;
        case FORMAT_YUYV422: return AV_PIX_FMT_YUYV422;
        case FORMAT_UYVY422: return AV_PIX_FMT_UYVY422;
        case FORMAT_YUVJ420: return AV_PIX_FMT_YUVJ420P;
        case FORMAT_YUVJ422: return AV_PIX_FMT_YUVJ422P;
        case FORMAT_YUVJ444: return AV_PIX_FMT_YUVJ444P;

        default: return AV_PIX_FMT_NONE;
    }
}


AVSampleFormat _2ffmpeg_format(SampleFormat f)
{
	switch (f)
	{
	case SampleFormat::U8: return AV_SAMPLE_FMT_U8;
	case SampleFormat::S16: return AV_SAMPLE_FMT_S16;
	case SampleFormat::FLT: return AV_SAMPLE_FMT_FLT;
	case SampleFormat::U8P: return AV_SAMPLE_FMT_U8P;
	case SampleFormat::S16P: return AV_SAMPLE_FMT_S16P;
	case SampleFormat::FLTP: return AV_SAMPLE_FMT_FLTP;

	default: return AV_SAMPLE_FMT_NONE;
	}
}

SampleFormat ffmpeg2format(AVSampleFormat f)
{
	switch (f)
	{
	case AV_SAMPLE_FMT_U8: return SampleFormat::U8;
	case AV_SAMPLE_FMT_S16: return SampleFormat::S16;
	case AV_SAMPLE_FMT_FLT: return SampleFormat::FLT;
	case AV_SAMPLE_FMT_U8P: return SampleFormat::U8P;
	case AV_SAMPLE_FMT_S16P: return SampleFormat::S16P;
	case AV_SAMPLE_FMT_FLTP: return SampleFormat::FLTP;

	default: return SampleFormat::NONE;
	}
}

int sample_size(SampleFormat f)
{
	switch (f)
	{
	case SampleFormat::U8: return 1;
	case SampleFormat::S16: return 2;
	case SampleFormat::FLT: return 4;
	case SampleFormat::U8P: return 1;
	case SampleFormat::S16P: return 2;
	case SampleFormat::FLTP: return 4;

	default: return 0;
	}
}
