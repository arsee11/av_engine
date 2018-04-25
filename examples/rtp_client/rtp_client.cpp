// rtp_client.cpp 

#ifdef WIN32

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"swscale.lib")
#pragma comment(lib, "jrtplib_d.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib,"sdl2.lib")
#pragma comment(lib, "av_engine.lib")
#endif

#include <iostream>

using namespace std;

#define SDL_MAIN_HANDLED

#include "av_rtp_source.h"
#include "av_decode_filter.h"
#include "av_exception.h"
//#include "../av_displayer.h"
#include "h264_rtp_packer.h"
#include <av_log.h>

int main(int argc, char* argv[])
{
#ifdef WIN32
	WSAData wsaData;
	::WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

	av_init();
	av_set_logger(stdout_log);

	try {
//		AVDisplayer dis;
//	        dis.open();
		AvDecodeFilter* df = AvDecodeFilter::create(CodecID::H264, nullptr);
		AvRtpSource<H264RtpPacker>* rtp = AvRtpSource<H264RtpPacker>::create(df, 8002, H264RtpPacker(25));
		
        while(true)
        {
            rtp->read();
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

