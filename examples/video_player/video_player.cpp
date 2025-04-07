// video_player.cpp 


#include <iostream>


#define SDL_MAIN_HANDLED

#include <av_frame_scale_filter.h>
#include <av_exception.h>
#include <av_file_source.h>
#include <av_video_decode_filter.h>
#include <av_video_decode_filter_nv.h>
#include <av_log.h>
#include "../av_dump.h"

using namespace std;

simplelogger::Logger* logger = simplelogger::LoggerFactory::CreateConsoleLogger();

int main(int argc, char* argv[])
{
    if(argc <2){
        cout<<"usage: player [file]\n";
        return 1;
    }
    const char* file_name = argv[1];
	av_init();
	av_set_logger(stdout_log);

	try {

		AvFileSource* avfile = AvFileSource::create();
		avfile->open(file_name);
		//avfile->open("rtmp://192.168.56.101/live/1");
		//avfile->open("rtsp://admin:HIBP123456@192.168.1.64");

		CodecInfo ci = avfile->codec_info(MediaType::MEDIA_VIDEO);
        AVDump d("out.rgb24");
		AvFrameScaleFilter* fs = AvFrameScaleFilter::create(PixelFormat::FORMAT_RGB24
            ,ci.w, ci.h, &d
        );
		//AvVideoDecodeFilter* df = AvVideoDecodeFilter::create(CodecID::CODEC_ID_NONE, fs);
		AvVideoDecodeFilterNv* df = AvVideoDecodeFilterNv::create(CodecID::CODEC_ID_NONE, fs);
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

