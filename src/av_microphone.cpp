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
#ifdef _MSC_VER
    AVInputFormat *ifmt=av_find_input_format("dshow");
    AVDictionary* options = NULL;    
    av_dict_set_int(&options, "sample_rate", sr, 0);
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
    av_dict_set_int(&options, "sample_rate", sr, 0);
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
    	
    AVCodecParameters *par = _format_ctx->streams[_stream_idx]->codecpar;  
    
	_param.format = ffmpeg2format((AVSampleFormat)par->format);
	_param.sr = par->sample_rate;
	_param.nchn = channels;
	_param.type = MEDIA_AUDIO;
	_param.codecid = _ffmpeg2codec(par->codec_id);
    _pack = av_packet_alloc();

	return true;    
}


void AvMicrophone::close()
{
    if (_format_ctx != nullptr)
    {
        avformat_close_input(&_format_ctx);
        _format_ctx = nullptr;
    }
    if (_pack != nullptr) {
        av_packet_free(&_pack);
        _pack = nullptr;
    }

}

AVParam* AvMicrophone::get()
{
	while (true)
	{
        av_packet_unref(_pack);
		if (av_read_frame(_format_ctx, _pack) >= 0)
		{
			if (_pack->stream_index == _stream_idx)
			{
				_param.data(_pack->data, _pack->size);
				_param.nsamples = (_pack->size/sample_size((SampleFormat)_param.format))
                                / _format_ctx->streams[_stream_idx]->codecpar->channels;

                
				break;
			}
		}
	}

	return &_param;
}
