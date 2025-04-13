///av_audio_decode_filter.cpp

#include "av_audio_decode_filter.h"

extern "C" {
#include <libavutil/imgutils.h>
}

#include <iostream>
#include "av_log.h"
#include "codec_specify.h"

int AvAudioDecodeFilter::frame_size()
{
	if (_codec_ctx	!= nullptr)
	{
		return _codec_ctx->frame_size;
	}
	return 0;
}

bool AvAudioDecodeFilter::transform(AVParam* p)
{
	if (_codec_ctx == nullptr)
	{
		if (_codec_id == CODEC_ID_NONE)
			_codec_id = p->codecid;

		if (!open(_codec_id, _sr, _channels, _sample_fmt))
			return false;
	}

	_avpacket->data = p->data_ptr();
	_avpacket->size = p->size();	
	av_frame_unref(_avframe);
	decode(_avframe, _avpacket);
	_param.nsamples = _avframe->nb_samples;
	if (av_sample_fmt_is_planar((AVSampleFormat)_avframe->format))
	{
		int nchn = _avframe->ch_layout.nb_channels;
		int size = av_samples_get_buffer_size(
			NULL, nchn, _avframe->nb_samples, (AVSampleFormat)_avframe->format, 1);

		_param.data(_avframe->data[0], size);
	}
	else
		_param.data(_avframe->data[0], _avframe->linesize[0]);


	return true;
}

void AvAudioDecodeFilter::decode(AVFrame* frame, AVPacket* packet)
{
	int rc = avcodec_send_packet(_codec_ctx, packet);
	if (rc < 0 && rc != AVERROR(EAGAIN))
	{
		char estr[256];
		av_make_error_string(estr, 256, rc);
        av_log_error()<< "In AvAudioDecodeFilter::transform() "<<estr<<end_log();
		return;
	}

	rc = avcodec_receive_frame(_codec_ctx, frame);
	if (rc < 0 && rc != AVERROR(EAGAIN))
	{
		char estr[256];
		av_make_error_string(estr, 256, rc);
        av_log_error()<< "In AvAudioDecodeFilter::transform() "<<estr<<end_log();
    }	

}
bool AvAudioDecodeFilter::open(const CodecInfo& ci)
{
	_codec_id = ci.codec;
	_sr = ci.sr;
	_channels = ci.nchn;
	_sample_fmt = ci.sp_format;
	return open(_codec_id, _sr, _channels, _sample_fmt);
}

void AvAudioDecodeFilter::close()
{
    
	if (_codec_ctx == NULL)
	    avcodec_free_context(&_codec_ctx);

    if(_avpacket != nullptr)
        av_packet_free(&_avpacket);

    if(_avframe!= nullptr)
        av_frame_free(&_avframe);
}


bool AvAudioDecodeFilter::open(CodecID cid, int sr, int channels, SampleFormat sample_fmt)
{
	const AVCodec* codec = avcodec_find_decoder(_2ffmpeg_id(cid));
	if (codec == NULL)
	{
        av_log_error() << "AvAudioDecodeFilter::open() failed" << end_log();
		return false;
	}

	if(codec->type != AVMEDIA_TYPE_AUDIO)
	{
        av_log_error() << "AvAudioDecodeFilter::open() Not Audio codec type" << end_log();
		return false;
	}
	
	_codec_ctx = avcodec_alloc_context3(codec);
	av_channel_layout_default(&_codec_ctx->ch_layout, channels);
	_codec_ctx->sample_fmt=_2ffmpeg_format(sample_fmt);
	_codec_ctx->sample_rate=sr;
	if (_codec_ctx == NULL)
    {
		av_log_error() << "AvAudioDecodeFilter::open() avcodec_alloc_context3 failed" << end_log();
		return false;
	}

	if (avcodec_open2(_codec_ctx, codec, nullptr) < 0)
	{
        av_log_error() << "AvAudioDecodeFilter::open() avcodec_open2 failed" << end_log();
		return false;
	}

	_param.format = sample_fmt;
	_param.codecid= S16LE;
	_param.sr = sr;
	_param.nchn= channels;
	_param.type = MEDIA_AUDIO;
	_avpacket = av_packet_alloc();
	_avframe = av_frame_alloc();
	return true;
}
