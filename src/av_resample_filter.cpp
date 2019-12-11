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
    	if(p->format != _out_format 
    	  ||p->nchn != _out_channels
    	  ||p->sr != _out_sr
    	)
    	{
		if (_swr_ctx == nullptr)
			open(p->nchn, p->sr, p->format, _out_channels, _out_sr, _out_format);

		int64_t delay = 0;//swr_get_delay(_swr_ctx, p->sr);
		av_log_info()<<"delay:"<<delay<<end_log();
		av_log_info()<<"in srs:"<<p->sr<<end_log();
		//av_log_info()<<"resample::itransform(): delay="<<delay<<end_log();
		/// p->nsamples * _out_sr/p->sr
		int64_t dst_nsamples = av_rescale_rnd(delay+p->nsamples, _out_sr
			, p->sr, AV_ROUND_UP);

		av_log_info()<<"dst_nsamples :"<<dst_nsamples <<end_log();

		AVFrame *inframe = av_frame_alloc();
		av_samples_fill_arrays(inframe->data, inframe->linesize, p->data_ptr(), p->nchn
			,p->nsamples, _2ffmpeg_format((SampleFormat)p->format), 1);

		AVFrame *outframe = av_frame_alloc();
		av_samples_alloc(outframe->data, outframe->linesize, _out_channels
			,dst_nsamples, _2ffmpeg_format(_out_format), 1);

		int ret = swr_convert(_swr_ctx, outframe->data, dst_nsamples,
			(const uint8_t **)inframe->data, p->nsamples);
		
		if (ret < 0)
		{
			av_frame_free(&outframe);
			av_frame_free(&inframe);
			av_log_error()<<"Error while converting"<<end_log();
			return false;
		}
	
		if (av_sample_fmt_is_planar(_2ffmpeg_format(_out_format)))
		{
			int size = av_samples_get_buffer_size(NULL, _out_channels, dst_nsamples, _2ffmpeg_format(_out_format), 1);
			uint8_t* buf = new uint8_t[size];
			int n = 0;
			for (int i = 0; i<_out_channels; i++)
			{
				memcpy(buf + n, outframe->data[i], outframe->linesize[0]);
				n += outframe->linesize[0];
			}

			_param.data(buf, n);
			delete[] buf;
		}
		else
			_param.data(outframe->data[0], outframe->linesize[0]);

		_param.nsamples = dst_nsamples;
		av_frame_free(&outframe);
		av_frame_free(&inframe);
    	}
    	else
    	{
    	    	_param.data(p->data_ptr(), p->size());
    	}
    	
    	return true;
}


void AvResampleFilter::open(int in_channels, int in_sr, int in_format
	,int out_channels, int out_sr, SampleFormat out_format)
{
	_swr_ctx = swr_alloc();
	if (_swr_ctx == NULL) {
		throw AvException("Could not allocate resampler context\n", __FILE__, __LINE__);
	}

	/* set options */
	av_opt_set_int(_swr_ctx, "in_channel_count", in_channels, 0);
	av_opt_set_int(_swr_ctx, "in_sr", in_sr, 0);
	av_opt_set_sample_fmt(_swr_ctx, "in_sample_fmt", (AVSampleFormat)in_format, 0);
	av_opt_set_int(_swr_ctx, "out_channel_count", out_channels, 0);
	av_opt_set_int(_swr_ctx, "out_sr", out_sr, 0);
	av_opt_set_sample_fmt(_swr_ctx, "out_sample_fmt", _2ffmpeg_format(out_format), 0);

	int ret;
	if ((ret = swr_init(_swr_ctx)) < 0) {
		throw AvException("Could not allocate resampler context\n", __FILE__, __LINE__);
	}
}
