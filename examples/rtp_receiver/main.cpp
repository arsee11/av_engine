// main.cpp 

#include <iostream>

using namespace std;

#define SDL_MAIN_HANDLED

#include "av_rtp_source.h"
#include "av_video_decode_filter.h"
#include "av_audio_decode_filter.h"
#include "av_exception.h"
#include "h264_rtp_packer.h"
#include "pcma_rtp_packer.h"
#include <av_log.h>
#include <av_util.h>
#include <sink.h>

class BinFileSink:public Sink<AVParam>
{
public:	
	BinFileSink(const std::string& filename){
		_fp = fopen(filename.c_str(), "wb");
	}

	~BinFileSink(){
		fclose(_fp);
	}

	void put(AVParam* p)override{
		fwrite(p->data_ptr(), p->size(), 1, _fp);
		fflush(_fp);
	}

private:
	FILE* _fp=nullptr;
};

int main(int argc, char* argv[])
{
#ifdef WIN32
	WSAData wsaData;
	::WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

	av_init();
	av_set_logger(stdout_log);

	try {
		AvVideoDecodeFilter* df = AvVideoDecodeFilter::create(CodecID::H264, nullptr);
		AvRtpSource<H264RtpPacker>* rtp = AvRtpSource<H264RtpPacker>::create(df, 8002, H264RtpPacker(25));
		BinFileSink pcmfs("recv.pcm");
		AvAudioDecodeFilter* adf = AvAudioDecodeFilter::create(CodecID::PCMA, 8000, 2, SampleFormat::S16, &pcmfs);
		AvRtpSource<PcmaRtpPacker>* pcmartp = AvRtpSource<PcmaRtpPacker>::create(adf, 8000, PcmaRtpPacker(10, 2));
		
        while(true)
        {
            //rtp->read();
            pcmartp->read();
        }        
	}
	catch (AvException& e) {
		cout << e.what() << endl;
	}



#ifdef _MSC_VER
	::WSACleanup();
#endif

	return 0;
}

