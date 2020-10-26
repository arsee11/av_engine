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
#include <map>


class AvFileSource : public Source<AVParam>
{
public:
	static AvFileSource* create(Transformation<AVParam>* next = nullptr)
	{
		return new AvFileSource(next);
	}	

    void destroy(){ delete this; }
	void open(const std::string& filename);
	AVParam* get()override;
	void close();


	CodecInfo codec_info(MediaType mt) { return _codec_infos[mt]; }

private:
	AvFileSource(Transformation<Param>* ts, bool read = false)
		:Source(ts)
		,_format_ctx(NULL)
	{
	}

    ~AvFileSource()
    {
        close();
    }

    	
    void initParams();
	enum AvReadRet{ AV_EOF, AV_READ_OK };	
	AvReadRet readp(AVPacket* packet);

private:
	AVFormatContext* _format_ctx;
	std::string _name;
	int _videostream = -1;
    int _audiostream = -1;

	std::map<MediaType, CodecInfo> _codec_infos;
	AVPacket* _pack = nullptr;
};

#endif /*AV_FILE_SOURCE_H*/
