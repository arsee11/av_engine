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
#include <av_decode_filter.h>
#include <av_exception.h>
#include <av_file_source.h>
#include <av_encode_filter.h>

#include "../av_displayer.h"

int main(int argc, char* argv[])
{

	av_init();

	try {
		AVDisplayer dis;
        dis.open();
		AvDecodeFilter* df = AvDecodeFilter::create(CodecID::CODEC_ID_NONE, &dis);

		AvFileSource* avfile = AvFileSource::create(df);
		avfile->open("e:/test.asf");

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

