
///  av_audio_encode_filter.cpp
#include "av_audio_encode_filter.h"
#include "av_log.h"

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavutil/error.h>    
}

#include "codec_specify.h"

//static FILE* f=fopen("test.enc", "wb");

bool AvAudioEncodeFilter::transform(AVParam*& p)
{
	if(_codec_ctx == nullptr)
	{
		av_log_error()<<"_codec_ctx==nullptr: codec not opened"<<end_log();
		return false;
	}

    
	if( (SampleFormat)p->format != _format)
	{
		av_log_error()<<"sample forma is incompatible:this->fromat="<<_format<<",p->format="<<p->format<<end_log();
		return false;
	}

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
	frame->channel_layout = av_get_default_channel_layout(_codec_ctx->channels); 
	frame->sample_rate = _sample_rate;
	
	AVPacket* pack = av_packet_alloc();
	
	AVParam* tp = AVParam::create();

	bool isok = true;
	int i;
	//av_log_info()<<"nb_samples:"<<nb_samples<<", p->nb_samples:"<<p->nb_samples<<end_log();
	//av_log_info()<<"raw data len:"<<p->len<<end_log();
	//To Do: deal with "if p->nb_samples < nb_samples"
	for (i = 0; i + nb_samples <= p->nb_samples; i += nb_samples)
	{
		av_samples_copy(frame->data, tmp_frame->data, 0, i, nb_samples, _codec_ctx->channels, format);
		//pts = _samples_count * (1/_sample_rate)/ time_base
		frame->pts = av_rescale_q(_samples_count, AVRational{ 1, _sample_rate }, _codec_ctx->time_base);
		//av_log_info()<<"pts="<<frame->pts<<end_log();
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
			//av_log_error()<<"encode frame failed"<<end_log();
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
		p->codecid = _codec_id;
		tp->release();		
		//fwrite(p->getData(), p->len, 1, f);
		//fflush(f);
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
	{
		av_log_error()<<"audo encoder no found, codecid:"<<_codec_id<<end_log();
		return false;
	}

	_codec_ctx = avcodec_alloc_context3(codec);
	if (_codec_ctx == NULL)
	{
		av_log_error()<<"audo encoder alloc failed, codecid:"<<_codec_id<<end_log();
		return false;
	}

	_codec_ctx->sample_fmt = _2ffmpeg_format(format);
	_format = format;
	_codec_ctx->bit_rate = sample_rate * nb_channels;
	_codec_ctx->sample_rate = sample_rate;
	_sample_rate = sample_rate;

	_codec_ctx->channels = nb_channels; 
	_codec_ctx->channel_layout = av_get_default_channel_layout(nb_channels);
	if (avcodec_open2(_codec_ctx, codec, NULL) < 0)
	{
		av_log_error()<<"audo encoder open failed, codecid:"<<_codec_id<<end_log();
		avcodec_free_context(&_codec_ctx);
		_codec_ctx=nullptr;
		return false;
	}	

	return true;
}
