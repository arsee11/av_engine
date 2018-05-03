///av_audio_decode_filter.h

#ifndef AV_AUDIO_DECODE_FILTER_H
#define AV_AUDIO_DECODE_FILTER_H

#include "av_util.h"
#include "filter.h"
#include "codec_specify.h"
#include "flexible_buffer.h"

class AvAudioDecodeFilter:public Filter<AVParam>
{
public:
	static AvAudioDecodeFilter* create(CodecID cid
		,int sample_rate, int channels, SampleFormat sample_fmt
		,Transformation<Param>* next_filter = nullptr) 
	{
		return new AvAudioDecodeFilter(cid, sample_rate, channels, sample_fmt, next_filter); 
	}

private:
	AvAudioDecodeFilter(CodecID cid
		,int sample_rate, int channels, SampleFormat sample_fmt
		,Transformation<Param>* next_filter = nullptr
	)
		:Filter<AVParam>(next_filter)
		,_codec_id(cid)
		,_sample_rate(sample_rate)
		,_channels(channels)
		,_sample_fmt(sample_fmt)
		,_inbuf(1280)
	{
	}

	bool transform(AVParam*& p)override;
	bool open(CodecID cid
		,int sample_rate, int channels, SampleFormat sample_fmt);

	void dumpData(AVFrame* avframe);
	void decode(AVFrame* frame, AVPacket* packet);

private:

	CodecID _codec_id;
	int _sample_rate, _channels;
	SampleFormat _sample_fmt;
	AVCodecContext* _codec_ctx = nullptr;
	AVCodecParserContext* _parser = nullptr;
	FlexibleBuffer<uint8_t> _inbuf;
};
#endif/*AV_AUDIO_DECODE_FILTER_H*/
