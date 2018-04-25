#include "av_decode_filter.h"

extern "C" {
#include <libavutil/imgutils.h>
}

#include <iostream>
#include "av_log.h"

bool AvDecodeFilter::transform(AVParam*& p)
{
	if (_codec_ctx == nullptr)
	{
		if (_codec_id == CODEC_ID_NONE)
			_codec_id = p->codecid;

		if (!open(_codec_id, p->w, p->h))
			return false;
	}

	AVPacket *packet =av_packet_alloc();
	packet->data = p->getData();
	packet->size = p->len;
	int rc = avcodec_send_packet(_codec_ctx, packet);
	if (rc < 0)
	{
		char estr[256];
		av_make_error_string(estr, 256, rc);
                av_log_error()<< "in AvDecodeFilter::transform() "<<estr<<end_log();
		av_packet_unref(packet);
		p->len = 0;
		return false;
	}

	AVFrame* frame = av_frame_alloc();
	rc = avcodec_receive_frame(_codec_ctx, frame);
	if (rc == 0)
	{
		AVPixelFormat f = (AVPixelFormat)frame->format;
		p->w = frame->width;
		p->h = frame->height;
		p->len = 0;
		int frame_size = av_image_get_buffer_size(f
			,p->w, p->h, 32
		);

		p->resize(frame_size);
		av_image_copy_to_buffer(p->getData()
			, frame_size
			, frame->data
			, frame->linesize
			, f
			, frame->width
			, frame->height
			, 32);

		p->len = frame_size;
		p->format = ffmpeg2format(f);
	}
	else
	{
		p->len = 0;
		return false;
	}

	av_packet_free(&packet);
	av_frame_unref(frame);
	return true;
}


bool AvDecodeFilter::open(CodecID cid, int w, int h)
{
	AVCodec* codec = avcodec_find_decoder(_2ffmpeg_id(cid));
	if (codec == NULL)
	{
                av_log_error() << "AvDecodeFilter::open() failed" << end_log();
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

	return true;
}
