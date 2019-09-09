//av_file_source.h

#ifndef AV_FILE_SOURCE_H
#define AV_FILE_SOURCE_H

extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

#ifndef AV_EXCEPTION_H
#include "av_exception.h"
#endif

#ifndef AV_UTIL_H
#include "av_util.h"
#endif

#include "source.h"


class AvFileSource : public Source<AVParam>
{
public:
	static AvFileSource* create(Transformation<AVParam>* next = nullptr)
	{
		return new AvFileSource(next);
	}	

	enum AvReadRet{ AV_EOF, AV_READ_OK };	

	void open(const std::string& filename)throw(AvException);
	AVParam* get()override;
	void close();

	int framerate() { return _framerate; }
	int width() { return _width; }
	int height() { return _height; }
	const CodecInfo& codec_info()const { return _codec_info; }

private:
	AvFileSource(Transformation<Param>* ts, bool read = false)
		:Source(ts)
		,_format_ctx(NULL)
		,_width(0)
		,_height(0)
	{
		_codec_info.codecpar = NULL;
	}

    	void initParams() throw(AvException);
	AvReadRet readp(AVPacket* packet);

private:
	AVFormatContext* _format_ctx;
	std::string _name;
	AVCodecID _vcodecid, _acodecid;
	int _width;
	int _height;
	int _framerate = 0;
	int _videostream = 0;
    	int _audiostream = 0;

	CodecInfo _codec_info;
};

#endif /*AV_FILE_SOURCE_H*/
