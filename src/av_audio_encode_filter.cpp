
///  av_audio_encode_filter.cpp
#include "av_audio_encode_filter.h"

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavutil/error.h>    
}

#include "codec_specify.h"

bool AvAudioEncodeFilter::transform(AVParam*& p)
{
    if(_codec_ctx == nullptr)
       return false;
    
	if (p->format != _format)
		return false;

	AVSampleFormat format = _2ffmpeg_format(_format);
	p->framerate = _sample_rate;

	AVFrame* tmp_frame = av_frame_alloc();
	av_samples_fill_arrays(tmp_frame->data, tmp_frame->linesize, p->getData(), _codec_ctx->channels, p->nb_samples, format, 1);

	int nb_samples = _codec_ctx->frame_size;
	if (_codec_ctx->codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE)
		nb_samples = p->nb_samples;

    AVFrame *frame = av_frame_alloc();
	av_samples_alloc(frame->data, frame->linesize, _codec_ctx->channels, nb_samples, format, 1);
			
	frame->nb_samples = nb_samples;
	frame->format = format;
	frame->channel_layout = AV_CH_LAYOUT_STEREO;
	frame->sample_rate = _sample_rate;
	
	AVPacket* pack = av_packet_alloc();
	
	AVParam* tp = AVParam::create();

	bool isok = true;
	int i;
	for (i = 0; i + nb_samples <= p->nb_samples; i += nb_samples)
	{
		av_samples_copy(frame->data, tmp_frame->data, 0, i, nb_samples, _codec_ctx->channels, format);
		frame->pts = av_rescale_q(_samples_count, AVRational{ 1, _sample_rate }, _codec_ctx->time_base);
		_samples_count += nb_samples;

		int rc = avcodec_send_frame(_codec_ctx, frame);
		if (rc < 0 && AVERROR(EAGAIN) != rc)
		{
			isok = false;
			break;
		}

		rc = avcodec_receive_packet(_codec_ctx, pack);
		if (rc == 0)
		{
			tp->addData(pack->data, pack->size);
			tp->pts = pack->pts;
			tp->dts = pack->pts;
		}
		else if (AVERROR(EAGAIN) != rc)
		{			
			isok = false;
			break;
		}

	}
    
	av_packet_free(&pack);
	av_frame_unref(frame);
	av_frame_unref(tmp_frame);

	if (isok && tp->len > 0)
	{
		p->setData(tp->getData(), tp->len);
		p->pts = tp->pts;
		p->dts = tp->dts;
		tp->release();		
		return true;
	}
	else
	{
		tp->release();
		return false;
	}    
}

bool AvAudioEncodeFilter::open(int sample_rate, int nb_channels, SampleFormat format)
{
	AVCodec* codec = avcodec_find_encoder(_2ffmpeg_id(_codec_id));
	if (codec == NULL)
		return false;

	_codec_ctx = avcodec_alloc_context3(codec);
	if (_codec_ctx == NULL)
		return false;

	_codec_ctx->sample_fmt = _2ffmpeg_format(format);
	_format = format;
	_codec_ctx->bit_rate = 64000;
	_codec_ctx->sample_rate = sample_rate;
	_sample_rate = sample_rate;

	_codec_ctx->channel_layout = AV_CH_LAYOUT_STEREO;
	_codec_ctx->channels = av_get_channel_layout_nb_channels(_codec_ctx->channel_layout);
	if (avcodec_open2(_codec_ctx, codec, NULL) < 0)
	{
		return false;
	}	

	return true;
}