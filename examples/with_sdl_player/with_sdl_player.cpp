// ffmpeg_rtp_server.cpp 

#ifdef _MSC_VER
#pragma comment(lib, "av_engine.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"swscale.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib,"sdl2.lib")
#endif

#include <iostream>

using namespace std;

#define SDL_MAIN_HANDLED

#include <av_frame_scale_filter.h>
#include <av_exception.h>
#include <av_file_source.h>
#include <av_video_decode_filter.h>
#include <av_log.h>
#include "../av_displayer.h"

AVPacket pkt;

int main(int argc, char* argv[])
{

	av_init();
	avformat_network_init();
	av_set_logger(stdout_log);

	av_init_packet(&pkt);
	pkt.data = NULL;
	pkt.size = 0;

	try {
		AVDisplayer dis;
        
		AvVideoDecodeFilter* df = AvVideoDecodeFilter::create(CodecID::CODEC_ID_NONE, &dis);

		AvFileSource* avfile = AvFileSource::create(df);
		//avfile->open("./av.mp4");//replace this filename
		//avfile->open("rtmp://192.168.56.101/live/1");
		avfile->open("rtsp://admin:HIBP123456@192.168.1.64");

		dis.open(avfile->width(), avfile->height());
		df->open(avfile->codec_info());

        while(true)
        {
            int ret = avfile->read();
            if(ret < 0)
                break;
            
            av_sleep(1000/avfile->framerate());
        }
	}
	catch (AvException& e) {
		cout << e.what() << endl;
	}

	return 0;
}

