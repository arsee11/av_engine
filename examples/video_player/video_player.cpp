// video_player.cpp 

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

int main(int argc, char* argv[])
{
	av_init();
	av_set_logger(stdout_log);

	try {

		AvFileSource* avfile = AvFileSource::create();
		avfile->open("./av.mp4");//replace this filename
		//avfile->open("rtmp://192.168.56.101/live/1");
		//avfile->open("rtsp://admin:HIBP123456@192.168.1.64");

		CodecInfo ci = avfile->codec_info(MediaType::MEDIA_VIDEO);
		AVDisplayer dis;
		dis.open(ci.w, ci.h);
		AvFrameScaleFilter* fs = AvFrameScaleFilter::create(PixelFormat::FORMAT_RGB24
            ,ci.w, ci.h, &dis
        );
		AvVideoDecodeFilter* df = AvVideoDecodeFilter::create(CodecID::CODEC_ID_NONE, fs);
		df->open(ci);
        avfile->setNext(df);

        while(true)
        {
            int ret = avfile->read();
            if(ret < 0)
                break;
            
            av_sleep(1000/ci.fps);
        }

        avfile->destroy();
        df->destroy();
        fs->destroy();
	}
	catch (AvException& e) {
		cout << e.what() << endl;
	}

	return 0;
}

