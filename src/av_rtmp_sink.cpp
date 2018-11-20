//av_rtmp_sink.cpp

#include "av_rtmp_sink.h"
#include "av_log.h"

AvRtmpSink::AvRtmpSink(const std::vector<AvStreamInfo>& ss, const std::string& push_url) throw(AvRtmpSinkException)
	:_push_url(push_url)
	,_format_ctx(NULL)
{
	int ret = -1;
	if ((ret = avformat_alloc_output_context2(&_format_ctx, NULL, "flv", _push_url.c_str())) < 0)
		throw AvRtmpSinkException(ret, __RTMP__, __LINE__);

	try {
		for (auto i : ss)
			addStream(i);

		open();
	}
	catch (AvRtmpSinkException& e)
	{
		avformat_free_context(_format_ctx);
		_format_ctx = NULL;
		throw e;
	}
}

void AvRtmpSink::open()throw(AvRtmpSinkException)
{
	int ret=-1;
		
	if (!(_format_ctx->oformat->flags & AVFMT_NORTMP))
	{
		ret = avio_open(&_format_ctx->pb, _push_url.c_str(), AVIO_FLAG_WRITE);
		if (ret < 0) 
			throw AvRtmpSinkException(ret, __RTMP__, __LINE__);	
	}

	ret = avformat_write_header(_format_ctx, NULL);
	if (ret < 0) {
		char errstr[255];
		av_make_error_string(errstr, 256, ret);
		throw AvRtmpSinkException(errstr, __RTMP__, __LINE__);
	}
}

