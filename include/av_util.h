//av_util.h

#ifndef AV_UTIL_H
#define  AV_UTIL_H

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
}

#include "codec_specify.h"


typedef struct Param {

	CodecID codecid=CODEC_ID_NONE;
	MediaType type=MEDIA_NONE;
	size_t pts=0;
	int format=-1; //pixel format or sample format.
	union{
	    struct{
	        int w;
	        int h;
	        int fps;
	    };
	    struct{
	        int sr; //smaple_rate
	        int nchn;//number of channels.
	        int nsamples;//mumber of smaples per channel.
	    };
	};

	void data(unsigned char* d, int l) {
		_buf = d;
		_size = l;
	}	

	size_t size(){ return _size; }	
    unsigned char* data_ptr(){ return _buf; }
	void clear(){ _buf = nullptr; _size = 0; }

	void copyParams(const Param& o){
		codecid = o.codecid;
		type    = o.type;
		format	= o.format;
		pts     = o.pts;
		w       = o.w;
		h       = o.h;
		fps     = o.fps;
		sr      = o.sr;
		nchn	= o.nchn;
		nsamples= o.nsamples;
	}

	Param& operator=(const Param&)=delete;

private:
    unsigned char* _buf=nullptr;
	size_t _size=0;
}AVParam;

///@return 0 OK, or failed
inline int av_init()
{
	//av_register_all();
	//avcodec_register_all();	
	avdevice_register_all();
	avformat_network_init();
	return 0;
}

#include <chrono>
#include <thread>

inline void av_sleep(int ms)
{
	std::this_thread::sleep_for( std::chrono::milliseconds(ms) );
}

#endif /*AV_UTIL_H*/
