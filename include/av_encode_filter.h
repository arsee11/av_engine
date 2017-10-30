//  av_encode_filter.h


#ifndef AV_ENCODE_FILTER_H
#define AV_ENCODE_FILTER_H

#include "av_util.h"
#include "filter.h"
#include "codec_specify.h"

struct AVCodec;
struct AVCodecContext;

class AvEncodeFilter:public Filter<AVParam>
{
public:
    static AvEncodeFilter* create(CodecID cid, int framerate
		,Transformation<Param>* next_filter=nullptr)
	{
		return new AvEncodeFilter(cid, framerate, next_filter); 
	}

	
private:
	AvEncodeFilter(CodecID cid, int framerate, Transformation<Param>* next_filter = nullptr)
		:Filter<AVParam>(next_filter)
		,_codec_id(cid)
		,_framerate(framerate)
    {
    }	
   
	bool open(PixelFormat f, int with, int height, int framerate);
	bool transform(AVParam*& p)override;

private:
    CodecID _codec_id;
    AVCodecContext* _codec_ctx=nullptr;
	unsigned long _frame_count = 0;
	int _framerate=0;	
  };

#endif /* AV_ENCODE_FILTER_H */
