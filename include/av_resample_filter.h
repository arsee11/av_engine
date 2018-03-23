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
    static AvResampleFilter* create(int out_channels, int out_sample_rate, SampleFormat out_format, Filter<AVParam>* next_filter=nullptr){
		return new AvResampleFilter(out_channels, out_sample_rate, out_format, next_filter); }
    
private:
	AvResampleFilter(int out_channels, int out_sample_rate, SampleFormat out_format, Filter<AVParam>* next_filter=nullptr)
    :Filter<AVParam>(next_filter)
    ,_out_channels(out_channels)
	,_out_sample_rate(out_sample_rate)
	,_out_format(out_format)
    {
    }
    
	bool transform(AVParam*& p)override;

	void open(int in_channels, int in_sample_rate, int in_format
		, int out_channels, int out_sample_rate, SampleFormat out_format)throw(AvException);
   
    PixelFormat _format = PixelFormat::FORMAT_NONE;
	struct SwrContext* _swr_ctx =nullptr;

	int _out_channels;
	int _out_sample_rate;
	SampleFormat _out_format;
};

#endif /* AV_RESAMPLE_FILTER_H */
