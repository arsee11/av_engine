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
	AvRtpException(const char* whats)
		:AvException(whats)
	{
	}

	AvRtpException(int errorCode)
		:AvException(errorCode)
	{	
	}
};


class RtpWrapper
{	
public:
	typedef std::shared_ptr<RtpWrapper> rtp_wrapper_ptr_t;
	enum { 
		MAX_PACKET_SIZE= 8*1024,
		MAX_PAYLOAD_SIZE= MAX_PACKET_SIZE-12
	};

public:
	bool addPeer(const char* ip, uint16_t port);	
	void open(int localPort, int HZ=8000, uint8_t playloadType=96, uint8_t frameRate=30.0) throw(AvRtpException);
	void close() { _rtpSession.Destroy(); }
	int sendPacket(void* buf, size_t len);
	std::tuple<int, bool, uint8_t*> readPacket();
	int readFrame(uint8_t** frame);

private:
	std::string getErrorStr(int errcode){
		return  RTPGetErrorString(errcode);
	}

private:
	RTPSession _rtpSession;
};

#endif /*RTP_WRAPPER_H*/