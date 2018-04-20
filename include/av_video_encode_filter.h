//  av_encode_filter.h


#ifndef AV_ENCODE_FILTER_H
#define AV_ENCODE_FILTER_H

#include "av_util.h"
#include "filter.h"
#include "codec_specify.h"

struct AVCodec;
struct AVCodecContext;

class AvVideoEncodeFilter:public Filter<AVParam>
{
public:
	///@brief create a new AvVideoEncodFilter instance.
	///@param cid codec id, for example CodecID::H264.
	///@param framerate  framerate of video.
	///@param bitrate video bitrate.
	///@param gop goup of picture.
    	static AvVideoEncodeFilter* create(CodecID cid, int framerate, int bitrate, int gop
		,Transformation<Param>* next_filter=nullptr)
	{
		return new AvVideoEncodeFilter(cid, framerate, bitrate, gop, next_filter); 
	}

	
private:
	AvVideoEncodeFilter(CodecID cid, int framerate, int bitrate, int gop
		,Transformation<Param>* next_filter = nullptr
	)
		:Filter<AVParam>(next_filter)
		,_codec_id(cid)
		,_framerate(framerate)
		,_bitrate(bitrate)
		,_gop(gop)
    {
    }	
   
	bool open(PixelFormat f, int with, int height, int framerate);
	bool transform(AVParam*& p)override;

private:
    	CodecID _codec_id;
    	AVCodecContext* _codec_ctx=nullptr;
	unsigned long _frame_count = 0;
	int _framerate=0;	
	int _bitrate=256000;	
	int _gop=50;	
  };

#endif /* AV_ENCODE_FILTER_H */
