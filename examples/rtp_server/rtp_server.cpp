#ifdef _MSC_VER
#include "stdafx.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"swscale.lib")
#pragma comment(lib, "jrtplib_d.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "../Debug/av_engine_lib")
#endif

#include <iostream>

using namespace std;


#include <av_encode_filter.h>
#include <av_rtp_sink.h>
#include <av_frame_scale_filter.h>
#include <av_exception.h>
#include <camera.h>



int main(int argc, char* argv[])
{
#ifdef WIN32
	WSAData wsaData;
	::WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

	av_init();

	try {
		
		AvRtpSink* rtp = AvRtpSink::create();
		rtp->addPeer("172.16.3.25", 8000);
        AvEncodeFilter* ef = AvEncodeFilter::create(CodecID::H264, 30, rtp);
        AvFrameScaleFilter * pf = AvFrameScaleFilter::create(PixelFormat::FORMAT_YUV420, 320, 240, ef);

		Camera c(pf);
#ifdef WIN32
		c.open("video=USB2.0 Camera", 30, 320, 240);
#else
		c.open("0", 30, 320, 240);
#endif

        while(true)
        {
            c.read();
        }
	}
	catch (AvException& e) {
		cout << e.what() << endl;
	}


#ifdef WIN32
	::WSACleanup();
#endif

	return 0;
}
