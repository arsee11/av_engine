//rtp_wrapper.h

#ifndef RTP_WRAPPER_H
#define RTP_WRAPPER_H

#include <jrtplib3/rtpsession.h>
#include <jrtplib3/rtpipv4address.h>

#include <vector>
#include <list>
#include <memory>
#include <tuple>
#include <iostream>

#include "av_exception.h"


using namespace jrtplib;

class AvRtpException: public AvException
{
public:
	AvRtpException(const char* whats, const char* file=__FILE__, int line=__LINE__)
		:AvException(whats, file, line)
	{
	}

	AvRtpException(int errorCode, const char* file=__FILE__, int line=__LINE__)
		:AvException(errorCode, file, line)
	{	
	}
};


class RtpWrapper
{	
public:
	typedef std::shared_ptr<RtpWrapper> rtp_wrapper_ptr_t;
	enum { 
		MAX_PACKET_SIZE= 8*1024,
		MAX_PAYLOAD_SIZE= 1400
	};

public:
	bool addPeer(const char* ip, uint16_t port);	
	void open(int localPort, double timestamp_unit, int payload_type) throw(AvRtpException);
	void close() { _rtpSession.Destroy(); }
	int sendPacket(const void* buf, size_t len, bool mark, uint32_t timestamp);
	std::tuple<int, bool, uint8_t*> readPacket();
	int readFrame(uint8_t** frame);

private:
	std::string getErrorStr(int errcode){
		return  RTPGetErrorString(errcode);
	}

private:
	RTPSession _rtpSession;
	int _payload_type;
};

#endif /*RTP_WRAPPER_H*/
