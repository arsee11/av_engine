//  av_microphone.cpp


#include "av_microphone.h"
#include <system_error>
#include "av_log.h"

AvMicrophone::AvMicrophone(Transformation<Param>* ts)
	:Source(ts)
{

}

bool AvMicrophone::open(const char* dev, int sr, int sample_size, int channels)
{
#ifdef WIN32
    	AVInputFormat *ifmt=av_find_input_format("dshow");
    	AVDictionary* options = NULL;    
    	av_dict_set_int(&options, "sr", sr, 0);
	av_dict_set_int(&options, "sample_size", sample_size, 0);
	av_dict_set_int(&options, "audio_buffer_size", 100, 0);

#endif 

#ifdef MACOS
	AVInputFormat *ifmt = av_find_input_format("avfoundation");
	AVDictionary* options = NULL;
	av_dict_set(&options, "audio_device_index", dev, 0);
#endif

#ifdef LINUX
    	AVInputFormat *ifmt=av_find_input_format("alsa");
    	AVDictionary* options = NULL;    
    	av_dict_set_int(&options, "sr", sr, 0);
	av_dict_set_int(&options, "channels", channels, 0);
#endif

    	_format_ctx = avformat_alloc_context();
    	int ec;
    	if((ec=avformat_open_input(&_format_ctx, dev, ifmt, &options))!=0)
    	{
    	    av_log_error()<<"Couldn't open AvMicrophone."<<end_log();
    	    return false;
    	}    
    	
    	if( (ec=avformat_find_stream_info(_format_ctx, NULL)) < 0 )
    	{
    	    char buf[128];
    	    	av_make_error_string(buf, 128, ec);
    	    	av_log_output(LOGL_ERROR, (std::string("Couldn't open AvMicrophone. error:") + std::string(buf)).c_str());
    	    return false; // Couldn't find stream information
    	}
    	
    	for(uint32_t i=0; i<_format_ctx->nb_streams; i++)
    	{
    	    if(_format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
    	    {
    	        _stream_idx = i;
    	        break;
    	    }
    	}
    	
    	_codec_ctx = avcodec_alloc_context3(NULL);
    	AVCodecParameters *par = _format_ctx->streams[_stream_idx]->codecpar;
    	avcodec_parameters_to_context(_codec_ctx, par);
    	AVCodec* pCodec = avcodec_find_decoder(par->codec_id);
    	if( pCodec == NULL ) {
    	    	av_log_output(LOGL_ERROR, "Unsupported codec");
    	    return false; 
    	}
    	
    	if( avcodec_open2(_codec_ctx, pCodec, NULL) < 0 ) {
    	    	av_log_output(LOGL_ERROR, "Could not open codec");
    	    return false; 
    	}    
    
	_param.format = ffmpeg2format((AVSampleFormat)_codec_ctx->sample_fmt);
	_param.sr = _codec_ctx->sample_rate;
	_param.nchn = channels;
	_param.type = MEDIA_AUDIO;
	_param.codecid = _ffmpeg2codec(_codec_ctx->codec_id);
	return true;    
}


void AvMicrophone::close()
{
    avformat_close_input(&_format_ctx);
    avcodec_free_context(&_codec_ctx);
}


AVParam* AvMicrophone::get()
{
	AVPacket* packet = av_packet_alloc();
	AVFrame *avframe = av_frame_alloc();
	while (true)
	{
		if (av_read_frame(_format_ctx, packet) >= 0)
		{
			if (packet->stream_index == _stream_idx)
			{
				int ret = avcodec_send_packet(_codec_ctx, packet);
				if (ret != 0)
				{
					if (AVERROR(EAGAIN) == ret)
						continue;
					else
					{
						return nullptr;
					}
				}

				ret = avcodec_receive_frame(_codec_ctx, avframe);
				if (ret != 0)
				{
					if (AVERROR(EAGAIN) == ret)
						continue;
					else
					{
						return nullptr;
					}
				}
				//av_log_info()<<"microphone read frame"<<end_log();
	
				if (av_sample_fmt_is_planar((AVSampleFormat)avframe->format))
				{
					int nchn = av_frame_get_channels(avframe);
					int size = av_samples_get_buffer_size(
						NULL, nchn, avframe->nb_samples, (AVSampleFormat)avframe->format, 1);

					uint8_t* buf = new uint8_t[size];
					int n = 0;
					for (int i = 0; i<nchn; i++)
					{
						memcpy(buf+n, avframe->data[i], avframe->linesize[0]);
						n += avframe->linesize[0];
					}
		
					_param.data(buf, n);
					delete[] buf;
				}
				else
					_param.data(avframe->data[0], avframe->linesize[0]);
				
				_param.nsamples = avframe->nb_samples;
				break;
			}
		}
	}

	av_packet_free(&packet);
	av_frame_free(&avframe);
	return &_param;
}
