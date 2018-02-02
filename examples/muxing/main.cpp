#ifdef _MSC_VER
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"swscale.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "../Debug/av_engine_lib")
#pragma comment(lib, "AsfFileWriter")
#endif

using namespace std;

#include <iostream>

#include <codec_specify.h>
#include <av_encode_filter.h>
#include <av_audio_encode_filter.h>
#include <av_file_sink.h>
#include <av_frame_scale_filter.h>
#include <av_resample_filter.h>
#include <av_exception.h>
#include <av_camera.h>
#include <av_microphone.h>
#include <av_log.h>

int main(int argc, char* argv[])
{
	av_init();
    av_set_logger(stdout_log);
	try {

		std::vector<AvStreamInfo> ss;
        ss.push_back(AvStreamInfo{ CodecID::MPEG4, MediaType::MEDIA_VIDEO,320,240});
        //ss.push_back(AvStreamInfo{ CodecID::AAC, MediaType::MEDIA_VIDEO, 44100, 1, SampleFormat::S16 });
		AvFileSink* avfile = AvFileSink::create(ss, "test.mp4");
		AvEncodeFilter* ef = AvEncodeFilter::create(CodecID::MPEG4, 10, avfile);
		AvFrameScaleFilter * pf = AvFrameScaleFilter::create(PixelFormat::FORMAT_YUV420, 320, 240, ef);
		
		AvCamera c(pf);
#ifdef WIN32
		c.open("video=USB2.0 AvCamera", 30, 640, 480);
		//c.open("video=HD Pro Webcam C920", 30, 320, 240);

#else
		c.open("0", 30, 320, 240);
#endif
		
		AvAudioEncodeFilter* aef = AvAudioEncodeFilter::create(CodecID::MP3, avfile);
		aef->open(44100, 1, SampleFormat::S16P);
		AvResampleFilter* rf = AvResampleFilter::create(2, 44100, SampleFormat::S16P, aef);
	
		AvMicrophone m(rf);

#ifdef WIN32
		m.open("audio=FrontMic (Realtek High Definiti", 44100, 16);
#else
		m.open("0", 44100, 16);
#endif

        for(int i=0; i<100; i++)
        {
            //m.read();
            c.read();
        }
        
		m.close();
		c.close();
		avfile->destroy();
	}
	catch (AvException& e) {
		cout << e.what() << endl;
	}

	return 0;
}
