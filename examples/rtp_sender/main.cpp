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


#include <av_video_encode_filter.h>
#include <av_audio_encode_filter.h>
#include <av_rtp_sink.h>
#include <av_frame_scale_filter.h>
#include <av_exception.h>
#include <av_camera.h>
#include <av_microphone.h>
#include <av_log.h>
#include <pcma_rtp_packer.h>
#include <h264_rtp_packer.h>
#include <av_resample_filter.h>


void send_audio()
{
}

void send_vidio()
{
}

int main(int argc, char* argv[])
{
#ifdef WIN32
	WSAData wsaData;
	::WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

	av_init();
	av_set_logger(stdout_log);
    
	try {
		const char* peer = "127.0.0.1";

		int vr=25; //video framerate
		AvRtpSink<H264RtpPacker>* rtp = AvRtpSink<H264RtpPacker>::create(9002, H264RtpPacker(vr));
		rtp->addPeer(peer, 8002);
		int w=320, h=240;
		AvVideoEncodeFilter* ef = AvVideoEncodeFilter::create(CodecID::H264, vr, w*h*3, 2*vr, rtp);
        	AvFrameScaleFilter * pf = AvFrameScaleFilter::create(PixelFormat::FORMAT_YUV420, w, h, ef);

		AvCamera c(pf);
#ifdef WIN32
		c.open("video=USB2.0 AvCamera", 30, w, h);
#endif

#ifdef LINUX 
		c.open("/dev/video0", 30, w, h);
#endif

#ifdef MACOS
		c.open("0", 30, w, h);
#endif


		int sr=8000;
		int channels=2;
		SampleFormat af=SampleFormat::S16;
		AvRtpSink<PcmaRtpPacker>* rtpa = AvRtpSink<PcmaRtpPacker>::create(9000, PcmaRtpPacker(10, channels));
		rtpa->addPeer(peer, 8000);
		AvAudioEncodeFilter* aef = AvAudioEncodeFilter::create(CodecID::PCMA, rtpa);
		aef->open(sr, channels, af);
		AvResampleFilter* rf = AvResampleFilter::create(channels, sr, af, aef);
		AvMicrophone m(rf);
#ifdef WIN32

#endif

#ifdef MACOS

#endif

#ifdef LINUX
		m.open("hw:0", 48000, 16);
#endif

        	while(true)
       		{
     	     		//m.read();
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
