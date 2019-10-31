
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

bool AvAudioEncodeFilter::transform(AVParam* p)
{
	if(_codec_ctx == nullptr)
	{
		av_log_error()<<"_codec_ctx==nullptr: codec not opened"<<end_log();
		return false;
	}

    
	if( (SampleFormat)p->format != _format)
	{
		av_log_error()<<"sample format is incompatible:this->fromat="<<_format<<",p->format="<<p->format<<end_log();
		return false;
	}

	if( p->sr!= _sample_rate)
	{
		av_log_error()<<"sample rate is incompatible:this ="<<_sample_rate<<",p="<<p->sr<<end_log();
		return false;
	}

	AVSampleFormat format = _2ffmpeg_format(_format);

	AVFrame* tmp_frame = av_frame_alloc();
	av_samples_fill_arrays(tmp_frame->data, tmp_frame->linesize, p->data_ptr(), _codec_ctx->channels, p->nsamples, format, 1);

	int nsamples = _codec_ctx->frame_size;
	if (_codec_ctx->codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE)
		nsamples = p->nsamples;

    	AVFrame *frame = av_frame_alloc();
	av_samples_alloc(frame->data, frame->linesize, _codec_ctx->channels, nsamples, format, 1);
			
	frame->nb_samples= nsamples;
	frame->format = format;
	frame->channel_layout = av_get_default_channel_layout(_codec_ctx->channels); 
	frame->sample_rate= _sample_rate;
	
	AVPacket* pack = av_packet_alloc();
	
	bool isok = true;
	int i;
	//av_log_info()<<"nsamples:"<<nsamples<<", p->nsamples:"<<p->nsamples<<end_log();
	//av_log_info()<<"raw data len:"<<p->len<<end_log();
	//To Do: deal with "if p->nsamples < nsamples"
	for (i = 0; i + nsamples <= p->nsamples; i += nsamples)
	{
		av_samples_copy(frame->data, tmp_frame->data, 0, i, nsamples, _codec_ctx->channels, format);
		//pts = _samples_count * (1/_sr)/ time_base
		frame->pts = av_rescale_q(_samples_count, AVRational{ 1, _sample_rate }, _codec_ctx->time_base);
		av_log_info()<<"pts="<<frame->pts<<end_log();
		_samples_count += nsamples;

		int rc = avcodec_send_frame(_codec_ctx, frame);
		if (rc < 0 && AVERROR(EAGAIN) != rc)
		{
			isok = false;
			break;
		}

		rc = avcodec_receive_packet(_codec_ctx, pack);
		if (rc == 0)
		{
			_param.data(pack->data, pack->size);
			_param.pts = pack->pts;
			_param.nsamples = nsamples;
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

	if (isok)
	{
		//fwrite(p->getData(), p->len, 1, f);
		//fflush(f);
		return true;
	}
	else
	{
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
	_codec_ctx->bit_rate = sample_rate* nb_channels;
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

	_param.format = format;
	_param.codecid = _codec_id;
	_param.sr = sample_rate;
	_param.nchn = nb_channels;
	_param.type = MEDIA_AUDIO;
	return true;
}
