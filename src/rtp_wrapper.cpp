///rtp_wrapper.cpp

#include <jrtplib3/rtpudpv4transmitter.h>
#include <jrtplib3/rtpsessionparams.h>
#include <jrtplib3/rtperrors.h>
#include <jrtplib3/rtppacket.h>

#include <iostream>

#include "rtp_wrapper.h"
#include "h264_rtp_packer.h"
#include "av_log.h"

	
void RtpWrapper::open(int local_port, double timestamp_unit, int payload_type)
{
	RTPUDPv4TransmissionParams transparams;
	RTPSessionParams sessparams;

	sessparams.SetOwnTimestampUnit(timestamp_unit);
	sessparams.SetAcceptOwnPackets(true);
	sessparams.SetMaximumPacketSize(MAX_PACKET_SIZE);
	transparams.SetPortbase(local_port);
	transparams.SetRTPReceiveBuffer(65536);
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

//static FILE* fr = fopen("rtp_recv.packet", "wb");
std::tuple<uint32_t, bool, int> RtpWrapper::readPacket(void* buf, int len)
{
	int size = 0;
	bool marker = false;
	uint32_t timestamp=0;
	if (_rtpSession.GotoFirstSourceWithData())
	{
		RTPPacket *rtpck;
		if ((rtpck = _rtpSession.GetNextPacket()) != NULL)
		{
			//cout<<"SequenceNumber:"<<rtpck->GetExtendedSequenceNumber()<<endl;
			//cout<<"SSRC:"<<rtpck->GetSSRC()<<endl;	
			//cout<<"Timestamp:"<<rtpck->GetTimestamp()<<endl;
			timestamp = rtpck->GetTimestamp();
			size = rtpck->GetPayloadLength();
			marker = rtpck->HasMarker();
			//cout << "marker=" << marker << endl;
			memcpy(buf, rtpck->GetPayloadData(), size); 
			_rtpSession.DeletePacket(rtpck);
			//fwrite(buf, size, 1, fr);
			//fflush(fr);
		}			
	}

#ifndef RTP_SUPPORT_THREAD
	int status=0;
	if(  (status=_rtpSession.Poll()) <0 )
		throw AvRtpException( status );
#endif 

	return std::make_tuple(timestamp, marker, size) ;
}
