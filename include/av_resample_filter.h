//av_resample_filter.h


#ifndef AV_RESAMPLE_FILTER_H
#define AV_RESAMPLE_FILTER_H

#include "filter.h"
#include "av_util.h"
#include "av_exception.h"

struct SwrContext;

class AvResampleFilter:public Filter<AVParam>
{
public:
    	static AvResampleFilter* create(int out_channels, int out_sr, SampleFormat out_format, Filter<AVParam>* next_filter=nullptr){
		return new AvResampleFilter(out_channels, out_sr, out_format, next_filter); }
    
private:
	AvResampleFilter(int out_channels, int out_sr, SampleFormat out_format, Filter<AVParam>* next_filter=nullptr)
    		:Filter<AVParam>(next_filter)
    		,_out_channels(out_channels)
		,_out_sr(out_sr)
	,_out_format(out_format)
    	{
		_param.format = _out_format;
		_param.nchn = _out_channels;
		_param.sr = _out_sr;
		_param.type = MEDIA_AUDIO;
    	}
    
	bool transform(AVParam* p)override;

	void open(int in_channels, int in_sr, int in_format
		, int out_channels, int out_sr, SampleFormat out_format)throw(AvException);
   
	struct SwrContext* _swr_ctx =nullptr;

	int _out_channels;
	int _out_sr;
	SampleFormat _out_format;
};

#endif /* AV_RESAMPLE_FILTER_H */
