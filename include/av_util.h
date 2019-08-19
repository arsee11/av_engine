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

    static Param* create(){ return new Param();}
    
    void release(){ delete this;}
	
	int len;
	int w;
	int h;
	int format; //SampleFormat or PixelFormat
	int framerate;
	int64_t pts;
	int64_t dts;
	CodecID codecid;
	int flag;
	int64_t nb_samples;
	int channels;
	int sample_rate;
	MediaType type;

	void setData(unsigned char* d, int l) {
		if (data == NULL)
		{
			data = (unsigned char*)malloc(l);
			data_size = l;
		}

		if (l > data_size)
		{
			free(data);
			data = (unsigned char*)malloc(l);
			data_size = l;
		}

		len = l;
		memcpy(data, d, l);
	}	

	void addData(unsigned char* d, int l) {
		if (data == NULL)
		{
			data = (unsigned char*)malloc(l);
			data_size = l;
		}

		if (len+l > data_size)
		{
			data_size = len + l;
			data = (unsigned char*)realloc(data, len + l);
		}

		memcpy(data+len, d, l);
		len += l;
	}

	unsigned char* getData(){ return data; }

	void resize(int size) {
		if (data != NULL)
			free(data); 
		if (size > 0)
		{
			data = (unsigned char*)malloc(size);
			data_size = size;
		}

		len = 0;
		
	}
private:
    Param()
		:data(NULL)
		,len(0)
		,w(0)
		,h(0)
		,data_size(0)
		,framerate(0)
		,pts(0)
		,dts(0)
		,flag(0)
		,nb_samples(0)
		,channels(0)
		,sample_rate(0)
		,type(MediaType::MEDIA_NONE)
	{}

    ~Param(){ 
		if(data != NULL )
			delete [] data;
	}
	Param(const Param&);
	Param& operator=(const Param&);

	int data_size;
	unsigned char* data;
	
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
