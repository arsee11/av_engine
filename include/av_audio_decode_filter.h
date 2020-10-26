///av_audio_decode_filter.h

#ifndef AV_AUDIO_DECODE_FILTER_H
#define AV_AUDIO_DECODE_FILTER_H

#include "av_util.h"
#include "filter.h"
#include "codec_specify.h"

class AvAudioDecodeFilter:public Filter<AVParam>
{
public:
	static AvAudioDecodeFilter* create(CodecID cid
			,int sr, int channels, SampleFormat sample_fmt
			,Transformation<Param>* next_filter = nullptr) 
	{
		return new AvAudioDecodeFilter(cid, sr, channels, sample_fmt, next_filter); 
	}

	static AvAudioDecodeFilter* create(Transformation<Param>* next_filter = nullptr)
	{
		return new AvAudioDecodeFilter(next_filter);
	}

    void destroy(){ delete this; }

	int frame_size();
	bool open(const CodecInfo& ci);
	void close();


private:
	AvAudioDecodeFilter(CodecID cid
		,int sr, int channels, SampleFormat sample_fmt
		,Transformation<Param>* next_filter = nullptr
	)
		:Filter<AVParam>(next_filter)
		,_codec_id(cid)
		,_sr(sr)
		,_channels(channels)
		,_sample_fmt(sample_fmt)
	{
	}

	AvAudioDecodeFilter(Transformation<Param>* next_filter = nullptr)
		:Filter<AVParam>(next_filter)
	{
	}

	~AvAudioDecodeFilter()
    {
        close();
    }

	bool transform(AVParam* p)override;
	bool open(CodecID cid, int sr, int channels, SampleFormat sample_fmt);
	void decode(AVFrame* frame, AVPacket* packet);

private:

	CodecID _codec_id;
	int _sr, _channels;
	SampleFormat _sample_fmt;
	AVCodecContext* _codec_ctx = nullptr;
	AVCodecParserContext* _parser = nullptr;	
	AVPacket* _avpacket =nullptr;
	AVFrame* _avframe = nullptr;
};
#endif/*AV_AUDIO_DECODE_FILTER_H*/
