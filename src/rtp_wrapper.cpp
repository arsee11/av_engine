///rtp_wrapper.cpp

#include <jrtplib3/rtpudpv4transmitter.h>
#include <jrtplib3/rtpsessionparams.h>
#include <jrtplib3/rtperrors.h>
#include <jrtplib3/rtppacket.h>

#include <iostream>

#include "rtp_wrapper.h"
#include "h264_rtp_packer.h"
#include "av_log.h"

using namespace std;

class Buffer
{
	struct buffer_item{
		uint32_t len;
		uint8_t * data;
	};

public:
	Buffer()
		:_size(0)
	{

	}
	~Buffer()
	{
		for(auto i: _buf)
			delete[] i.data;
	}

	///@brief Own the data, release them in Destructor
	void Push(uint8_t* data, uint32_t len)
	{
		buffer_item item={len, data};
		_buf.push_back( item );
		_size += len;
	}

	std::tuple<int32_t, uint8_t*>  Retrieve()
	{
		uint32_t ret=0;
		uint8_t* data = new uint8_t[ get_size() ];
		for(auto i : _buf)
		{
			memcpy(data+ret, i.data, i.len);
			ret += i.len;
		}

		return std::make_tuple(ret, data);
	}

	uint32_t get_size()
	{
		return _size;
	}

private:
	
	std::list<buffer_item> _buf;
	uint32_t _size;
};
	
void RtpWrapper::open(int local_port, double timestamp_unit, int payload_type) throw(AvRtpException)
{
	RTPUDPv4TransmissionParams transparams;
	RTPSessionParams sessparams;

	sessparams.SetOwnTimestampUnit(timestamp_unit);
	sessparams.SetAcceptOwnPackets(true);
	transparams.SetPortbase(local_port);
	sessparams.SetMaximumPacketSize(MAX_PACKET_SIZE);
	int ret = _rtpSession.Create(sessparams,&transparams);		
	if(ret < 0 )
		throw AvRtpException( getErrorStr(ret).c_str(), __FILE__, __LINE__ );

	_rtpSession.SetDefaultPayloadType(payload_type);
	_payload_type = payload_type;

}

bool RtpWrapper::addPeer(const char* ip, uint16_t port)
{
	uint32_t ip4;
	int ret = inet_pton(AF_INET, ip, &ip4);
	if (ret != 1)
		return false;

	ip4 = ntohl(ip4);
	ret = _rtpSession.AddDestination(RTPIPv4Address(ip4, port));
	if (ret < 0)
		return false;

	return true;
}

int RtpWrapper::sendPacket(const void* buf, size_t len, bool mark, uint32_t timestamp)
{
	int ret = _rtpSession.SendPacket( buf, len, _payload_type, mark, timestamp);
	if(ret < 0)
		av_log_error()<<getErrorStr(ret)<<end_log();

	return ret;
}

std::tuple<int, bool, uint8_t*> RtpWrapper::readPacket()
{
	int ret = 0;
	bool marker = false;
	uint8_t* buf = nullptr;
	if (_rtpSession.GotoFirstSourceWithData())
	{
		RTPPacket *rtpck;
		if ((rtpck = _rtpSession.GetNextPacket()) != NULL)
		{
			//cout<<"SequenceNumber:"<<rtpck->GetExtendedSequenceNumber()<<endl;
			//cout<<"SSRC:"<<rtpck->GetSSRC()<<endl;	
			//cout<<"Timestamp:"<<rtpck->GetTimestamp()<<endl;
			ret = rtpck->GetPayloadLength();
			marker = rtpck->HasMarker();
			//cout << "marker=" << marker << endl;
			buf = new uint8_t[ret];
			memcpy(buf, rtpck->GetPayloadData(), rtpck->GetPayloadLength());				
			_rtpSession.DeletePacket(rtpck);
		}			
	}

#ifndef RTP_SUPPORT_THREAD
	int status=0;
	if(  (status=_rtpSession.Poll()) <0 )
		throw AvRtpException( status );
#endif 

	return std::make_tuple(ret, marker, buf) ;
}

int RtpWrapper::readFrame(uint8_t** frame)
{
	Buffer buf;
	uint8_t *data = nullptr;
	int ret =0;
	bool marker=false;
	int count = 0;
	_rtpSession.BeginDataAccess();
	while( !marker )
	{
		std::tie(ret, marker, data) = readPacket();
		if (ret > 0 && data != nullptr)
		{
			buf.Push(data, ret);
			count++;
		}

	}
	_rtpSession.EndDataAccess();
	uint32_t len=0;
	std::tie(len, *frame) = buf.Retrieve();
	return len;
}
