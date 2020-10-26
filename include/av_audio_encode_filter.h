//  av_audio_encode_filter.h


#ifndef AV_AUDIO_ENCODE_FILTER_H
#define AV_AUDIO_ENCODE_FILTER_H

#include "av_util.h"
#include "filter.h"
#include "codec_specify.h"

struct AVCodec;
struct AVCodecContext;

class AvAudioEncodeFilter:public Filter<AVParam>
{
public:
    	static AvAudioEncodeFilter* create(CodecID cid, Transformation<AVParam>* next_filter=nullptr){
		return new AvAudioEncodeFilter(cid, next_filter); 
	}

	void destroy() { delete this; }

	bool open(int sr, int nb_channels, SampleFormat format);
	void close();

	bool transform(AVParam* p)override;

private:
	AvAudioEncodeFilter(CodecID cid, Transformation<Param>* next_filter = nullptr)
		:Filter<AVParam>(next_filter)
		,_codec_id(cid)
    	{
    	}	
   
	~AvAudioEncodeFilter()
	{
		close();
	}

private:
    CodecID _codec_id;
    AVCodecContext* _codec_ctx=nullptr;
	int _sample_rate = 0;
	int _channel_layout = 0;
	SampleFormat _format = SampleFormat::NONE;
	int _samples_count = 0;
	AVPacket* _pack = nullptr;
  };

#endif /* AV_AUDIO_ENCODE_FILTER_H */
