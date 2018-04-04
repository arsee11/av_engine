///av_decode_filter.h

#ifndef AV_DECODE_FILTER_H
#define AV_DECODE_FILTER_H

#include "av_util.h"
#include "filter.h"
#include "codec_specify.h"

class AvDecodeFilter:public Filter<AVParam>
{
public:
	static AvDecodeFilter* create(CodecID cid=CODEC_ID_NONE, Transformation<Param>* next_filter = nullptr) {
		return new AvDecodeFilter(cid, next_filter); }

private:
	AvDecodeFilter(CodecID cid, Transformation<Param>* next_filter = nullptr)
		:Filter<AVParam>(next_filter)
		,_codec_id(cid)
	{
	}

	bool transform(AVParam*& p)override;

	bool open(CodecID cid, int w, int h);

private:

	CodecID _codec_id;
	AVCodecContext* _codec_ctx = nullptr;
};
#endif/*AV_DECODE_FILTER_H*/