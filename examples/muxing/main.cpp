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
#include <av_video_encode_filter.h>
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

		std::string fname = {"test.asf"};
		std::vector<AvStreamInfo> ss;

		int w=640, h=480;
		CodecID vc=CodecID::H264;
        	ss.push_back(AvStreamInfo{ vc, MediaType::MEDIA_VIDEO,w,h});

		CodecID ac = CodecID::PCMA;
		SampleFormat af=SampleFormat::S16;
		int sr=48000;
		AvStreamInfo as;
		as.codecid=ac;
		as.media_type=MediaType::MEDIA_AUDIO;
		as.ai.sample_rate=sr;
		as.ai.channel=1;
		as.ai.sample_format=af;
        	ss.push_back(as);
		AvFileSink* avfile = AvFileSink::create(ss, fname); 
		AvVideoEncodeFilter* ef = AvVideoEncodeFilter::create(vc, 10, w*h*3, 10*2, avfile); 
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

		
		AvAudioEncodeFilter* aef = AvAudioEncodeFilter::create(ac, avfile);
		aef->open(sr, 1, af);
		AvResampleFilter* rf = AvResampleFilter::create(1, sr, af, aef);
	
		AvMicrophone m(rf);

#ifdef WIN32
		m.open("audio=FrontMic (Realtek High Definiti", 44100, 16);
#endif

#ifdef LINUX
		m.open("hw:0", 48000, 16);
#endif

        	for(int i=0; i<300; i++)
        	{
        	    //m.read();
        	    c.read();
        	}
        
		m.close();
		//c.close();
		avfile->destroy();
	}
	catch (AvException& e) {
		cout << e.what() << endl;
	}

	return 0;
}
