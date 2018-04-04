//av_rtp_sink.h

#ifndef AV_RTP_OUTPUT_FORMAT_H
#define AV_RTP_OUTPUT_FORMAT_H


#include "rtp_wrapper.h"
#include "av_exception.h"
#include "av_util.h"
#include "sink.h"
#include <memory>
#include "av_log.h"

using namespace jrtplib;

class AvRtpOutputFormatException: public AvException
{
public:
	AvRtpOutputFormatException(const char* whats, const char* file=__FILE__, int line=__LINE__)
		:AvException(whats, file, line)
	{
	}

	AvRtpOutputFormatException(int errorCode, const char* file=__FILE__, int line=__LINE__)
		:AvException(errorCode, file, line)
	{	
	}
};

///@brief send rtp packets to network.
template<class RtpPacker>
class AvRtpSink:public Sink<AVParam>
{
	typedef RtpWrapper::rtp_wrapper_ptr_t rtp_wrapper_ptr_t;

public:
	static AvRtpSink* create(uint16_t localPort, const RtpPacker& pk)
	{
		return new AvRtpSink(localPort, pk); 
	}

	static AvRtpSink* create(rtp_wrapper_ptr_t& rtp)
	{
		return new AvRtpSink(rtp);
	}

	void put(AVParam* p)override
	{
		auto pkgs = std::move(_rtp_packer.pack( p->getData(), p->len ));
		for(auto&& i : pkgs)
		{	
			av_log_info()<<"send rtp pack: payload len="<<i.len()<<end_log();
			_rtp->sendPacket(i.data(), i.len(), i.mark(), i.timestamp());
		}
	}

	void addPeer(const char* ip, uint16_t port)
	{
		_rtp->addPeer(ip, port);
	}	

private:
	AvRtpSink(uint16_t localPort, const RtpPacker& pk) 
		:_rtp_packer(pk)
	{
		_rtp.reset(new RtpWrapper); 
		_rtp->open(localPort, pk.TimestampUnit(), pk.payload_type());
	}

	AvRtpSink(rtp_wrapper_ptr_t& rtp)
		:_rtp(rtp)
	{
	}	

private:
	RtpPacker _rtp_packer;
	rtp_wrapper_ptr_t  _rtp;
};

#endif /*AV_RTP_OUTPUT_FORMAT_H*/
