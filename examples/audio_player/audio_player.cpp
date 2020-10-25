//audio_player.cpp 

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
#include <av_audio_decode_filter.h>
#include <av_log.h>

#include <sink.h>
#include <SDL.h>
#include <mutex>
#include "../third_party/include/flexible_buffer.h"


class AvAudioPlayer : public Sink<AVParam>
{
public:
	static AvAudioPlayer* create(const std::string& devname)
	{
		return new AvAudioPlayer(devname);		
	}

	void open(int sample_rate, int channels, SampleFormat format);
	void close();


	void put(AVParam* p)override;

	~AvAudioPlayer() {
		close();
	}

private:
	AvAudioPlayer(const std::string& devname)
		:_devname(devname)
		,_buf(_max_buf_size)
	{
	}

	SDL_AudioFormat toFormat(SampleFormat format);
	static void  play_callback(void* udata, Uint8* data, int size);
	void onRequestAudio(Uint8* data, int size);
	void pushAudioBuffer(const uint8_t* data, int size);

private:
	std::string  _devname;
	using locker_t = std::lock_guard<std::mutex>;
	std::mutex _buf_mutex;
	arsee::FlexibleBuffer<uint8_t> _buf;
	int _max_buf_size = 48000*2; //1 second 48KHZ
};

void AvAudioPlayer::open(int sample_rate, int channels, SampleFormat format)
{
	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
		printf("Could not initialize SDL - %s\n", SDL_GetError());
		return;
	}
	SDL_AudioSpec wanted_spec;
	wanted_spec.freq = sample_rate;
	wanted_spec.format = toFormat(format);
	wanted_spec.channels = channels;
	wanted_spec.silence = 0;
	wanted_spec.samples = 100 * sample_rate / 1000; //100ms len
	wanted_spec.callback = &AvAudioPlayer::play_callback;
	wanted_spec.userdata = this;

	if (SDL_OpenAudio(&wanted_spec, NULL) < 0) {
		printf("can't open audio.\n");
		return;
	}
	SDL_PauseAudio(0);
}

void AvAudioPlayer::close()
{
	SDL_PauseAudio(1);
	SDL_Quit();
}

void AvAudioPlayer::pushAudioBuffer(const uint8_t* data, int size)
{
	locker_t lck(_buf_mutex);
	if (_buf.size() + size > _max_buf_size)
		size = _max_buf_size - _buf.size();
	
	if(size > 0)
		_buf.push(data, size);
}

void AvAudioPlayer::put(AVParam* p)
{
	if(p->type == MediaType::MEDIA_AUDIO)
		pushAudioBuffer(p->data_ptr(), p->size());
}

SDL_AudioFormat AvAudioPlayer::toFormat(SampleFormat format)
{
	switch (format)
	{
	case SampleFormat::U8: return AUDIO_U8;
	case SampleFormat::S16: return AUDIO_S16SYS;
	case SampleFormat::FLT: return AUDIO_F32SYS;
	case SampleFormat::U8P: return -1;
	case SampleFormat::S16P: return -1;
	case SampleFormat::FLTP: return -1;

	default: return -1;
	}
}

void  AvAudioPlayer::play_callback(void* udata, Uint8* data, int size) {
	AvAudioPlayer* player = static_cast<AvAudioPlayer*>(udata);
	if (player != nullptr)
	{
		player->onRequestAudio(data, size);
	}
}

void AvAudioPlayer::onRequestAudio(Uint8* data, int size)
{
	locker_t lck(_buf_mutex);

	SDL_memset(data, 0, size);
	int buf_size = _buf.size();
	if (buf_size == 0)
		return;

	printf("buf_size=%d, request size=%d\n", buf_size, size);
	size = (size > buf_size ? buf_size : size);
	SDL_MixAudio(data, _buf.begin(), size, SDL_MIX_MAXVOLUME);
	_buf.consume(size);
}

int main(int argc, char* argv[])
{
	av_init();
	av_set_logger(stdout_log);

	try {
		const char* devname = "";
		AvAudioPlayer* player = AvAudioPlayer::create(devname);
		AvAudioDecodeFilter* df = AvAudioDecodeFilter::create(player);

		AvFileSource* avfile = AvFileSource::create(df);
		avfile->open("./test.asf");//replace this filename
		//avfile->open("rtmp://192.168.56.101/live/1");

		CodecInfo ci = avfile->codec_info(MediaType::MEDIA_AUDIO);
		player->open(ci.sr
			,ci.nchn
			,ci.sp_format
		);
		df->open(ci);

		while (true)
		{
			int ret = avfile->read();
			if (ret < 0)
				break;

			av_sleep(500);
		}
		player->close();
	}
	catch (AvException& e) {
		cout << e.what() << endl;
	}

	return 0;
}

