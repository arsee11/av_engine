// av_resample_filter.cpp


#include "av_resample_filter.h"
#include <tuple>
#include "av_log.h"

extern "C" {
	#include <libswresample/swresample.h>
}

//static FILE* f=fopen("swr.pcm", "wb");

bool AvResampleFilter::transform(AVParam*& p)
{
    if(p->format != _out_format 
      ||p->channels != _out_channels
      ||p->sample_rate != _out_sample_rate
    )
    {
		if (_swr_ctx == nullptr)
			open(p->channels, p->sample_rate, p->format, _out_channels, _out_sample_rate, _out_format);

		int64_t delay = swr_get_delay(_swr_ctx, _out_sample_rate);
		//av_log_info()<<"resample::itransform(): delay="<<delay<<end_log();
		/// p->nb_samples * _out_sample_rate/p->sample_rate
		int64_t dst_nb_samples = av_rescale_rnd(delay+p->nb_samples,_out_sample_rate
			, p->sample_rate, AV_ROUND_UP);

		AVFrame *inframe = av_frame_alloc();
		av_samples_fill_arrays(inframe->data, inframe->linesize, p->getData(), p->channels
			,p->nb_samples, _2ffmpeg_format((SampleFormat)p->format), 1);

		AVFrame *outframe = av_frame_alloc();
		av_samples_alloc(outframe->data, outframe->linesize, _out_channels
			,dst_nb_samples, _2ffmpeg_format(_out_format), 1);

		int ret = swr_convert(_swr_ctx, outframe->data, dst_nb_samples,
			(const uint8_t **)inframe->data, p->nb_samples);
		
		if (ret < 0)
		{
			av_log_error()<<"Error while converting"<<end_log();
			return false;
		}
	
		if (av_sample_fmt_is_planar(_2ffmpeg_format(_out_format)))
		{
			int size = av_samples_get_buffer_size(NULL, _out_channels, dst_nb_samples, _2ffmpeg_format(_out_format), 1);
			uint8_t* buf = new uint8_t[size];
			int n = 0;
			for (int i = 0; i<_out_channels; i++)
			{
				memcpy(buf + n, outframe->data[i], outframe->linesize[0]);
				n += outframe->linesize[0];
			}

			p->setData(buf, n);
			delete[] buf;
		}
		else
			p->setData(outframe->data[0], outframe->linesize[0]);

		//fwrite(p->getData(), p->len, 1, f);
		//fflush(f);
		p->nb_samples = dst_nb_samples;
		p->format = _out_format;
		p->channels = _out_channels;
		p->sample_rate = _out_sample_rate;
		av_frame_free(&outframe);
		av_frame_free(&inframe);
    }
    
    return true;
}


void AvResampleFilter::open(int in_channels, int in_sample_rate, int in_format
	,int out_channels, int out_sample_rate, SampleFormat out_format)throw(AvException)
{
	_swr_ctx = swr_alloc();
	if (_swr_ctx == NULL) {
		throw AvException("Could not allocate resampler context\n", __FILE__, __LINE__);
	}

	/* set options */
	av_opt_set_int(_swr_ctx, "in_channel_count", in_channels, 0);
	av_opt_set_int(_swr_ctx, "in_sample_rate", in_sample_rate, 0);
	av_opt_set_sample_fmt(_swr_ctx, "in_sample_fmt", (AVSampleFormat)in_format, 0);
	av_opt_set_int(_swr_ctx, "out_channel_count", out_channels, 0);
	av_opt_set_int(_swr_ctx, "out_sample_rate", out_sample_rate, 0);
	av_opt_set_sample_fmt(_swr_ctx, "out_sample_fmt", _2ffmpeg_format(out_format), 0);

	int ret;
	if ((ret = swr_init(_swr_ctx)) < 0) {
		throw AvException("Could not allocate resampler context\n", __FILE__, __LINE__);
	}
}
