//av_rtp_source.h

#ifndef AV_RTP_SOURCE_H
#define AV_RTP_SOURCE_H

#include <assert.h>

#include "av_exception.h"
#include "av_util.h"
#include "rtp_wrapper.h"
#include "source.h"

using namespace jrtplib;

class AvRtpSourceException: public AvException
{
public:
	AvRtpSourceException(const char* whats)
		:AvException(whats)
	{
	}

	AvRtpSourceException(int errorCode)
		:AvException(errorCode)
	{	
	}
};


///@brief receive rtp packets form network.
class AvRtpSource : public Source<AVParam>
{
	typedef AVParam param_t;
	typedef RtpWrapper::rtp_wrapper_ptr_t rtp_wrapper_ptr_t;

public:
	static AvRtpSource* create(Transformation<Param>* ts, uint16_t localPort=8000)
	{
		return new AvRtpSource(ts, localPort);
	}

	rtp_wrapper_ptr_t getRtpWrapper() const { return _rtp; }

protected:
	bool transform(AVParam*& p)override {
		uint8_t* buf = nullptr;
		int size = _rtp->readFrame(&buf);
		if (size > 0)
		{
			_param->setData(buf, size);
			p = _param;
			return true;
		}

		return false;
	}

private:
	AvRtpSource(Transformation<Param>* ts, uint16_t localPort=8000)
		:Source(ts)
		,_localPort(localPort)
		,_rtp(nullptr)
	{
		_rtp = rtp_wrapper_ptr_t(new RtpWrapper); 
		_rtp->open(_localPort);
	}

	AvRtpSource(Transformation<Param>* ts, rtp_wrapper_ptr_t rtp)
		:Source(ts)
		,_localPort(0)
		,_rtp(rtp)
	{
	}

private:
	rtp_wrapper_ptr_t _rtp;
	uint16_t _localPort;
	param_t* _param = param_t::create();
};

#endif /*AV_RTP_SOURCE_H*/