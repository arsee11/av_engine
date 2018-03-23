//av_rtp_sink.h

#ifndef AV_RTP_OUTPUT_FORMAT_H
#define AV_RTP_OUTPUT_FORMAT_H


#include "rtp_wrapper.h"
#include "av_exception.h"
#include "av_util.h"
#include "Sink.h"

using namespace jrtplib;

class AvRtpOutputFormatException: public AvException
{
public:
	AvRtpOutputFormatException(const char* whats)
		:AvException(whats)
	{
	}

	AvRtpOutputFormatException(int errorCode)
		:AvException(errorCode)
	{	
	}
};

///@brief send rtp packets to network.
class AvRtpSink:public Sink<AVParam>
{
	typedef RtpWrapper::rtp_wrapper_ptr_t rtp_wrapper_ptr_t;

public:
	static AvRtpSink* create(uint16_t localPort, uint8_t playload_type, int HZ, uint8_t framerate)
	{
		return new AvRtpSink(localPort, HZ, playload_type, framerate);
	}

	static AvRtpSink* create(rtp_wrapper_ptr_t& rtp)
	{
		return new AvRtpSink(rtp);
	}

	void put(AVParam* p)override
	{
		_rtp->sendPacket(p->getData(), p->len);
	}

	void addPeer(const char* ip, uint16_t port)
	{
		_rtp->addPeer(ip, port);
	}	

private:
	AvRtpSink(uint16_t localPort, int HZ, uint8_t playload_type, uint8_t framerate)
		:_rtp(nullptr)
	{
		_rtp = rtp_wrapper_ptr_t(new RtpWrapper); 
		_rtp->open(localPort, HZ, playload_type, framerate);
	}

	AvRtpSink(rtp_wrapper_ptr_t& rtp)
		:_rtp(rtp)
	{
	}	

private:
	rtp_wrapper_ptr_t  _rtp;
};

#endif /*AV_RTP_OUTPUT_FORMAT_H*/
