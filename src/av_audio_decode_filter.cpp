///av_audio_decode_filter.cpp

#include "av_audio_decode_filter.h"

extern "C" {
#include <libavutil/imgutils.h>
}

#include <iostream>
#include "av_log.h"
#include "codec_specify.h"

bool AvAudioDecodeFilter::transform(AVParam* p)
{
	if (_codec_ctx == nullptr)
	{
		if (_codec_id == CODEC_ID_NONE)
			_codec_id = p->codecid;

		if (!open(_codec_id, _sr, _channels, _sample_fmt))
			return false;
	}

	uint8_t* data=p->data_ptr();
	int data_size=p->size();
	AVPacket *packet =av_packet_alloc();
	AVFrame* avframe = av_frame_alloc();	
	_inbuf.clear();

	while (data_size > 0)
	{
		int ret = data_size;
		packet->data = data;
		packet->size = data_size;
		//int ret = av_parser_parse2(_parser, _codec_ctx
		//	,&packet->data, &packet->size
		//        ,data, data_size
		//        ,AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);

		if (ret < 0)
		{
		    av_log_error()<< "in AvAudioDecodeFilter::transform() Error while parsing"<<end_log();
		    break;
		}
	       	decode(avframe, packet);
				
		data += ret;
        	data_size -= ret;
	}
	
	
	av_packet_free(&packet);
	if(_inbuf.size() == 0)
	{
		av_frame_unref(avframe);
		return false;
	}

	_param.nsamples = avframe->nb_samples;
	_param.data(_inbuf.begin(), _inbuf.size() );

	av_frame_unref(avframe);

	return true;
}

void AvAudioDecodeFilter::decode(AVFrame* frame, AVPacket* packet)
{
	int rc = avcodec_send_packet(_codec_ctx, packet);
	if (rc < 0)
	{
		char estr[256];
		av_make_error_string(estr, 256, rc);
                av_log_error()<< "in AvAudioDecodeFilter::transform() "<<estr<<end_log();
		return;
	}

	while (rc >= 0)
	{
		rc = avcodec_receive_frame(_codec_ctx, frame);
		if (rc == AVERROR(EAGAIN) || rc == AVERROR_EOF)
            		return;
        	else if (rc < 0)
		{
			char estr[256];
			av_make_error_string(estr, 256, rc);
                	av_log_error()<< "in AvAudioDecodeFilter::transform() "<<estr<<end_log();
           		return;
        	}
		
		dumpData(frame);
	}	

}

void AvAudioDecodeFilter::dumpData(AVFrame* avframe)
{
	av_log_info()<<"audio decode "<<avframe->linesize[0]<<end_log();
	if (av_sample_fmt_is_planar((AVSampleFormat)avframe->format))
	{
		int nchn = av_frame_get_channels(avframe);
		int size = av_samples_get_buffer_size(
			NULL, nchn, avframe->nb_samples, (AVSampleFormat)avframe->format, 1);

		for (int i = 0; i<nchn; i++)
			_inbuf.push(avframe->data[i], avframe->linesize[0]);
	}
	else
		_inbuf.push(avframe->data[0], avframe->linesize[0]);
	
}


bool AvAudioDecodeFilter::open(CodecID cid, int sr, int channels, SampleFormat sample_fmt)
{
	AVCodec* codec = avcodec_find_decoder(_2ffmpeg_id(cid));
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
	_codec_ctx->channels=channels;
	_codec_ctx->channel_layout=av_get_default_channel_layout(channels);
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
	_param.sr = sr;
	_param.nchn= channels;
	_param.type = MEDIA_AUDIO;
	return true;
}
