#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"swscale.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "../Debug/av_engine_lib")
#pragma comment(lib, "AsfFileWriter")
#include <iostream>

using namespace std;

#include <codec_specify.h>
#include <av_encode_filter.h>
#include <av_audio_encode_filter.h>
#include <av_file_sink.h>
#include <av_frame_scale_filter.h>
#include <av_resample_filter.h>
#include <av_exception.h>
#include <camera.h>
#include <microphone.h>

int main(int argc, char* argv[])
{
	av_init();


	try {

		std::vector<AvStreamInfo> ss;
		ss.push_back(AvStreamInfo{ CodecID::H264 });
		ss.push_back(AvStreamInfo{ CodecID::MP3 });
		AvFileSink* avfile = AvFileSink::create(ss, "E:\\test.asf");
		AvEncodeFilter* ef = AvEncodeFilter::create(CodecID::H264, 10, avfile);
		AvFrameScaleFilter * pf = AvFrameScaleFilter::create(PixelFormat::FORMAT_YUV420, 640, 480, ef);
		
		Camera c(pf);
#ifdef WIN32
		c.open("video=USB2.0 Camera", 30, 640, 480);
		//c.open("video=HD Pro Webcam C920", 30, 320, 240);

#else
		c.open("0", 30, 320, 240);
#endif
		
		AvAudioEncodeFilter* aef = AvAudioEncodeFilter::create(CodecID::MP3, avfile);
		aef->open(44100, 1, SampleFormat::S16P);
		AvResampleFilter* rf = AvResampleFilter::create(2, 44100, SampleFormat::S16P, aef);
	
		Microphone m(rf);

#ifdef WIN32
		m.open("audio=FrontMic (Realtek High Definiti", 44100, 16);
#else
		m.open("0", 44100, 16);
#endif

        for(int i=0; i<1000; i++)
        {
            m.read();
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