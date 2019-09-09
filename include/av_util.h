//av_util.h

#ifndef AV_UTIL_H
#define  AV_UTIL_H

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
}

#include "codec_specify.h"
#include <../third_party/include/flexible_buffer.h>


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
	        int nsamples;//mumber of smaples.
	    };
	};

	void data(const unsigned char* d, int l) {
		_buf.push(d, l);
	}	

	size_t size(){ return _buf.size(); }	

	unsigned char* data_ptr(){ return _buf.begin(); }
	void clear(){ _buf.clear(); }

	Param():_buf(256){};

private:
	arsee::FlexibleBuffer<unsigned char> _buf;

}AVParam;

///@return 0 OK, or failed
inline int av_init()
{
	av_register_all();
	avcodec_register_all();	
	avdevice_register_all();
	avformat_network_init();
	return 0;
}

#ifdef _MSC_VER
#define _WINSOCKAPI_
#include <Windows.h>
#undef _WINSOCKAPI_
#else
#include <unistd.h> //unix or linux
#endif

inline void av_sleep(int ms)
{
#ifdef _MSC_VER
	Sleep(ms);
#else
    usleep(ms*1000);
#endif


}

#endif /*AV_UTIL_H*/
