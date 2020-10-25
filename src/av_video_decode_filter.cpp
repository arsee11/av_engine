#include "av_video_decode_filter.h"

extern "C" {
#include <libavutil/imgutils.h>
}

#include <iostream>
#include "av_log.h"

bool AvVideoDecodeFilter::transform(AVParam* p)
{
	if (_codec_ctx == nullptr)
	{
		if (_codec_id == CODEC_ID_NONE)
			_codec_id = p->codecid;

		if (!open(_codec_id, p->w, p->h))
			return false;
	}


	AVPacket *packet =av_packet_alloc();
	packet->data = p->data_ptr();
	packet->size = p->size();
	int rc = avcodec_send_packet(_codec_ctx, packet);
	if (rc < 0)
	{
		char estr[256];
		av_make_error_string(estr, 256, rc);
                av_log_error()<< "in AvVideoDecodeFilter::transform() "<<estr<<end_log();
		av_packet_unref(packet);
		return false;
	}

	AVFrame* frame = av_frame_alloc();
	rc = avcodec_receive_frame(_codec_ctx, frame);
	if (rc == 0)
	{
		AVPixelFormat f = (AVPixelFormat)frame->format;
		int frame_size = av_image_get_buffer_size(f
			,p->w, p->h, 32
		);

		//_param.resize(frame_size);
		uint8_t* buf = new uint8_t[frame_size];
		av_image_copy_to_buffer(buf
			, frame_size
			, frame->data
			, frame->linesize
			, f
			, frame->width
			, frame->height
			, 32);

        _param.data(buf, frame_size);
		delete[] buf;
		_param.format = ffmpeg2format(f);

	}
	else
	{
		return false;
	}

	av_packet_free(&packet);
	av_frame_unref(frame);
	return true;
}


bool AvVideoDecodeFilter::open(CodecID cid, int w, int h)
{
	AVCodec* codec = avcodec_find_decoder(_2ffmpeg_id(cid));
	if (codec == NULL)
	{
                av_log_error() << "AvVideoDecodeFilter::open() failed" << end_log();
		return false;
	}

	if(codec->type != AVMEDIA_TYPE_VIDEO)
	{
                av_log_error() << "AvVideoDecodeFilter::open() Not Video codec type" << end_log();
		return false;
	}

	_codec_ctx = avcodec_alloc_context3(codec);
	if (_codec_ctx == NULL)
		return false;

	_codec_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
	AVRational rate;
	rate.num =0;
	rate.den = 1;
	_codec_ctx->framerate = rate;
	_codec_ctx->thread_count = 4;
	_codec_ctx->width = w;
	_codec_ctx->height = h;

	AVDictionary* opts = NULL;
	av_dict_set(&opts, "allow_sw", "1", 0); //allows software encoding
	if (avcodec_open2(_codec_ctx, codec, &opts) < 0)
	{
		return false;
	}

	_param.fps = rate.den;
	_param.w= w;
	_param.h= h;
    _param.codecid = CodecID::CODEC_ID_NONE;
	_param.type = MEDIA_VIDEO;
	_param.format = ffmpeg2format(_codec_ctx->pix_fmt);
	return true;
}

bool AvVideoDecodeFilter::open(const CodecInfo& ci)
{
	_codec_id = ci.codec;
	AVCodec* codec = avcodec_find_decoder(_2ffmpeg_id(_codec_id));
	if (codec == NULL)
	{
		av_log_error() << "AvVideoDecodeFilter::open() failed" << end_log();
		return false;
	}

	if (codec->type != AVMEDIA_TYPE_VIDEO)
	{
		av_log_error() << "AvVideoDecodeFilter::open() Not Video codec type" << end_log();
		return false;
	}

	_codec_ctx = avcodec_alloc_context3(codec);
	if (_codec_ctx == NULL)
		return false;

	avcodec_parameters_to_context(_codec_ctx, static_cast<AVCodecParameters*>(ci.codecpar));
	AVDictionary* opts = NULL;
	av_dict_set(&opts, "allow_sw", "1", 0); //allows software encoding
	if (avcodec_open2(_codec_ctx, codec, &opts) < 0)
	{
		return false;
	}

	_param.fps = ci.fps;
	_param.w = ci.w;
	_param.h= ci.h;
	_param.codecid = _codec_id;
	_param.format = ci.pix_format;
	_param.type = MediaType::MEDIA_VIDEO;
	return true;
}
