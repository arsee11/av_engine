// av_resample_filter.cpp


#include "av_resample_filter.h"
#include <tuple>
#include "av_log.h"

extern "C" {
	#include <libswresample/swresample.h>
}

bool AvResampleFilter::transform(AVParam* p)
{
    _param.nsamples = p->nsamples;
    if( p->format != _out_format 
      ||p->nchn != _out_channels
      ||p->sr != _out_sr
    )
    {
		if (_swr_ctx == nullptr)
			open(p->nchn, p->sr, (SampleFormat)p->format, _out_channels, _out_sr, _out_format);

		AVFrame *inframe = av_frame_alloc();
		av_samples_fill_arrays(inframe->data, inframe->linesize, p->data_ptr(), p->nchn
			,p->nsamples, _2ffmpeg_format((SampleFormat)p->format), 1);

		/// p->nsamples * _out_sr/p->sr
		int64_t dst_nsamples = av_rescale_rnd(p->nsamples, _out_sr, p->sr, AV_ROUND_UP);
		if (dst_nsamples != _out_nsamples)
		{
			if(_out_nsamples > 0)
				av_freep(_outframe->data);

			av_samples_alloc(_outframe->data, _outframe->linesize, _out_channels
							,dst_nsamples, _2ffmpeg_format(_out_format), 1);
			_out_nsamples = dst_nsamples;
		}
		int ret = swr_convert(_swr_ctx, _outframe->data, dst_nsamples
							 ,(const uint8_t **)inframe->data, p->nsamples);
		
		if (ret < 0)
		{
			av_frame_free(&inframe);
			av_log_error()<<"Error while converting"<<end_log();
			return false;
		}
	
		_param.data(_outframe->data[0], _outframe->linesize[0]);
		_param.nsamples = dst_nsamples;
		av_frame_free(&inframe);
    }
   	else
   	{
    	_param.data(p->data_ptr(), p->size());
   	} 
    	
    return true;
}


void AvResampleFilter::open(int in_channels, int in_sr, SampleFormat in_format
	,int out_channels, int out_sr, SampleFormat out_format)
{
	_swr_ctx = swr_alloc_set_opts(NULL
			,out_channels
			,_2ffmpeg_format(out_format)
			,out_sr
			,in_channels
			, _2ffmpeg_format(in_format)
			,in_sr
			,0, NULL);
	if (_swr_ctx == nullptr)
	{
		throw AvException("Could not allocate resampler context\n", __FILE__, __LINE__);
	}

	int ret;
	if ((ret = swr_init(_swr_ctx)) < 0)
	{
		throw AvException("Could not allocate resampler context\n", __FILE__, __LINE__);
	}

	_outframe = av_frame_alloc();
}

void AvResampleFilter::close()
{
	if (_swr_ctx != nullptr)
	{
		swr_free(&_swr_ctx);
		_swr_ctx = nullptr;
	}
	if (_outframe != nullptr) 
	{
		av_frame_free(&_outframe);
		_outframe = nullptr;
	}

}
