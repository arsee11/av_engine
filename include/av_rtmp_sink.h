//av_rtmp_sink.h

#ifndef AV_RTMP_SINK_H
#define AV_RTMP_SINK_H

extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

#include "av_file_sink.h"

class AvRtmpSinkException: public AvException
{
public:
	AvRtmpSinkException(const char* whats, const char* rtmp, int line)
		:AvException(whats,  rtmp, line)
	{
	}

	AvRtmpSinkException(int errorCode, const char* rtmp, int line)
		:AvException(errorCode, rtmp, line)
	{	
	}
};



class AvRtmpSink : public AvFileSink
{	
public:
	static AvRtmpSink* create(const std::vector<AvStreamInfo>& ss
		,const std::string& push_url)
	{
		try {
			return new AvRtmpSink(ss, push_url);
		}catch (AvRtmpSinkException&) {
			return nullptr;
		}
	}


	void destroy() { delete this; }

private:
	AvRtmpSink(const std::vector<AvStreamInfo>& ss
		,const std::string& push_url)throw(AvRtmpSinkException);

	~AvRtmpSink() {
		close();
	}

	void addStream(const AvStreamInfo& s)throw(AvRtmpSinkException);
	void open()throw(AvRtmpSinkException);

	enum{ BUF_SIZE=1024*1240*4};



private:
	std::string  _push_url;
};

#endif /*AV_RTMP_SINK_H*/
