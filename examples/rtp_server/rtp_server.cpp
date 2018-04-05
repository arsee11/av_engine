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
#include <av_audio_encode_filter.h>
#include <av_rtp_sink.h>
#include <av_frame_scale_filter.h>
#include <av_exception.h>
#include <av_camera.h>
#include <av_microphone.h>
#include <av_log.h>
#include <pcma_rtp_packer.h>


int main(int argc, char* argv[])
{
#ifdef WIN32
	WSAData wsaData;
	::WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

	av_init();
	av_set_logger(stdout_log);
    
	try {
/*		AvRtpSink* rtp = AvRtpSink::create(9000, 96, 90000, 25);
		rtp->addPeer("172.16.3.25", 8000);
		AvEncodeFilter* ef = AvEncodeFilter::create(CodecID::H264,25, rtp);
        	AvFrameScaleFilter * pf = AvFrameScaleFilter::create(PixelFormat::FORMAT_YUV420, 320, 240, ef);

		AvCamera c(pf);
#ifdef WIN32
		c.open("video=USB2.0 Camera", 30, 320, 240);
#else
		c.open("0", 30, 320, 240);
#endif
*/
		int sr=48000;
		AvRtpSink<PcmaRtpPacker>* rtp = AvRtpSink<PcmaRtpPacker>::create(9000, PcmaRtpPacker(20, 2));
		rtp->addPeer("192.168.0.2", 8000);
		AvAudioEncodeFilter* ef = AvAudioEncodeFilter::create(CodecID::PCMA, rtp);
		ef->open(sr, 2, SampleFormat::S16);
		AvMicrophone m(ef);
#ifdef WIN32
		c.open("video=USB2.0 Camera", 30, 320, 240);
#endif

#ifdef MACOS
		c.open("0", 30, 320, 240);
#endif

#ifdef LINUX
		m.open("hw:0", sr, 16);
#endif
        	while(true)
       		{
       	     		m.read();
       	     		//c.read();
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
