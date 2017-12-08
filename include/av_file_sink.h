//av_file_sink.h

#ifndef AV_FILE_SINK_H
#define AV_FILE_SINK_H

extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}


#ifndef AV_EXCEPTION_H
#include "av_exception.h"
#endif

#include "transformation.h"
#include "av_util.h"
#include <vector>

class AvFileSinkException: public AvException
{
public:
	AvFileSinkException(const char* whats, const char* file, int line)
		:AvException(whats,  file, line)
	{
	}

	AvFileSinkException(int errorCode, const char* file, int line)
		:AvException(errorCode, file, line)
	{	
	}
};


struct AvStreamInfo
{
	CodecID codecid;
		union{
		struct{
			PixelFormat pixel_format;
			int width;
			int height;
		}vi;
		struct{
			int sample_rate;
			int channel;
			SampleFormat sample_format;
		}ai;
	};
};

class AvFileSink : public Transformation<AVParam>
{	
public:
	static AvFileSink* create(const std::vector<AvStreamInfo>& ss
		,const std::string& filename)
	{
		try {
			return new AvFileSink(ss, filename);
		}catch (AvFileSinkException&) {
			return nullptr;
		}
	}

	void destroy() { delete this; }

private:
	AvFileSink(const std::vector<AvStreamInfo>& ss
		,const std::string& filename)throw(AvFileSinkException);

	~AvFileSink() {
		close();
	}

	void addStream(const AvStreamInfo& s)throw(AvFileSinkException);
	void open()throw(AvFileSinkException);
	void close();

	enum{ BUF_SIZE=1024*1240*4};

	void write(AVPacket* packet) throw(AvFileSinkException);
	void put(AVParam* p)override;

	void get(AVParam* p)override {

	}

private:
	AVFormatContext* _format_ctx;
	std::string  _filename;
	CodecID _codec_id = CodecID::CODEC_ID_NONE;
	CodecID _audio_codec_id = CodecID::CODEC_ID_NONE;
	AVStream* _video_stream=nullptr;
	AVStream* _audio_stream=nullptr;
	uint32_t _frame_count = 1;
};

#endif /*AV_FILE_SINK_H*/
