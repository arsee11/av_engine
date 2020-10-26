#ifdef _MSC_VER
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"swscale.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "../../lib/av_engine.lib")
#endif

using namespace std;

#include <iostream>

#include <codec_specify.h>
#include <av_video_encode_filter.h>
#include <av_audio_encode_filter.h>
#include <av_video_decode_filter.h>
#include <av_file_sink.h>
#include <av_frame_scale_filter.h>
#include <av_resample_filter.h>
#include <av_exception.h>
#include <av_camera.h>
#include <av_microphone.h>
#include <av_log.h>

#ifdef _MSC_VER
#include <Windows.h>
string ANSItoUTF8(const char* strAnsi)
{
	//获取转换为宽字节后需要的缓冲区大小，创建宽字节缓冲区，936为简体中文GB2312代码页
	int nLen = MultiByteToWideChar(CP_ACP, NULL, strAnsi, -1, NULL, NULL);
	WCHAR* wszBuffer = new WCHAR[nLen + 1];
	nLen = MultiByteToWideChar(CP_ACP, NULL, strAnsi, -1, wszBuffer, nLen);
	wszBuffer[nLen] = 0;
	//获取转为UTF8多字节后需要的缓冲区大小，创建多字节缓冲区
	nLen = WideCharToMultiByte(CP_UTF8, NULL, wszBuffer, -1, NULL, NULL, NULL, NULL);
	CHAR* szBuffer = new CHAR[nLen + 1];
	nLen = WideCharToMultiByte(CP_UTF8, NULL, wszBuffer, -1, szBuffer, nLen, NULL, NULL);
	szBuffer[nLen] = 0;

	string s1 = szBuffer;
	//内存清理
	delete[]wszBuffer;
	delete[]szBuffer;
	return s1;
}

string UTF8toANSI(const char* strUTF8)
{
	//获取转换为多字节后需要的缓冲区大小，创建多字节缓冲区
	int nLen = MultiByteToWideChar(CP_UTF8, NULL, strUTF8, -1, NULL, NULL);
	WCHAR* wszBuffer = new WCHAR[nLen + 1];
	nLen = MultiByteToWideChar(CP_UTF8, NULL, strUTF8, -1, wszBuffer, nLen);
	wszBuffer[nLen] = 0;

	nLen = WideCharToMultiByte(936, NULL, wszBuffer, -1, NULL, NULL, NULL, NULL);
	CHAR* szBuffer = new CHAR[nLen + 1];
	nLen = WideCharToMultiByte(936, NULL, wszBuffer, -1, szBuffer, nLen, NULL, NULL);
	szBuffer[nLen] = 0;

	string s1 = szBuffer;
	//清理内存
	delete[]szBuffer;
	delete[]wszBuffer;
	return s1;
}
#endif //_MSC_VER



#define AUDIO
//#define VIDEO 
int main(int argc, char* argv[])
{
	av_init();
    av_set_logger(stdout_log);
	try {

		std::string fname = {"test.asf"};
		std::vector<AvStreamInfo> ss;

#ifdef VIDEO
		int w=320, h=240;
		CodecID vc=CodecID::H264;
		ss.push_back(AvStreamInfo{ vc, MediaType::MEDIA_VIDEO,w,h});
#endif

#ifdef AUDIO 
		CodecID ac = CodecID::PCMA;
		SampleFormat af=SampleFormat::S16;
		int sr=8000;
        int nchn=1;
		AvStreamInfo as;
		as.codecid=ac;
		as.media_type=MediaType::MEDIA_AUDIO;
		as.ai.sample_rate=sr;
		as.ai.channel=nchn;
		as.ai.sample_format=af;
        ss.push_back(as);
#endif

		AvFileSink* avfile = AvFileSink::create(ss, fname); 

#ifdef VIDEO
		AvVideoEncodeFilter* ef = AvVideoEncodeFilter::create(vc, 30, w*h*3, 10*2, avfile); 
		AvFrameScaleFilter * pf = AvFrameScaleFilter::create(PixelFormat::FORMAT_YUV420, w, h, ef);
		
		AvCamera c(nullptr);
#ifdef _MSC_VER
		c.open("video=USB2.0 HD UVC WebCam", 30, w, h);
#endif

#ifdef LINUX 
		c.open("/dev/video0", 30, w, h);
#endif

#ifdef MACOS
		c.open("0", 30, w, h);
#endif
	
		if (c.codec() != CodecID::CODEC_ID_NONE) {
			AvVideoDecodeFilter* df = AvVideoDecodeFilter::create(c.codec(), pf);
			c.setNext(df);
		}
#endif //VIDEO

#ifdef AUDIO 
		AvAudioEncodeFilter* aef = AvAudioEncodeFilter::create(ac, avfile);
		aef->open(sr, nchn, af);
		AvResampleFilter* rf = AvResampleFilter::create(nchn, sr, af, aef);
	
		AvMicrophone m(rf);

#ifdef _MSC_VER
		char* dev = "audio=麦克风 (Realtek(R) Audio)";
		std::string strdev = ANSItoUTF8(dev);
		m.open(strdev.c_str(), 44100, 16, 2);
		
#endif

#ifdef LINUX
		m.open("hw:1", 48000, 16, 1);
#endif

#endif //AUDIO

		for(int i=0; i<1000; i++)
        {

#ifdef AUDIO 
            m.read();
#endif
#ifdef VIDEO 
            c.read();
#endif
        }
       
#ifdef AUDIO 
		m.close();
#endif
#ifdef VIDEO 
		c.close();
#endif
        rf->destroy();
        aef->destroy();
		avfile->destroy();
	}
	catch (AvException& e) {
		cout << e.what() << endl;
	}
    

	return 0;
}
