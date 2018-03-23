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
	
void RtpWrapper::open(int local_port, int HZ, uint8_t playload_type, uint8_t framerate) throw(AvRtpException)
{
    _hz = HZ;
    _framerate = framerate;
    _payload_type = playload_type;
	RTPUDPv4TransmissionParams transparams;
	RTPSessionParams sessparams;

	sessparams.SetOwnTimestampUnit(1.0/framerate);
	sessparams.SetAcceptOwnPackets(true);
	transparams.SetPortbase(local_port);
	sessparams.SetMaximumPacketSize(MAX_PACKET_SIZE);
	int ret = _rtpSession.Create(sessparams,&transparams);		
	if(ret < 0 )
		throw AvRtpException( getErrorStr(ret).c_str() );

	_rtpSession.SetDefaultPayloadType(playload_type);

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

static std::tuple<uint8_t*, uint16_t> mediaPack(int media_type, uint8_t* data, int offset, int len, bool is_fragment, bool is_end )
{
    if(media_type == 96)
    {
        return H264RTPPacker::pack(data, offset, len, is_fragment, is_end);
    }
    
    return std::make_tuple(nullptr, 0);
}

int RtpWrapper::sendPacket(void* buf, size_t len)
{
	int ret = 0;
    uint8_t* bufs=nullptr;
    uint16_t buf_len=0;
	if(len <= MAX_PAYLOAD_SIZE)
    {
        std::tie(bufs, buf_len)= mediaPack(_payload_type, (uint8_t*)buf, 0, len, false, true);
        ret = _rtpSession.SendPacket(bufs, buf_len, _payload_type, true, _hz/_framerate);
        if(ret < 0)
            av_log_error()<<getErrorStr(ret)<<end_log();
        
        return ret;
    }
    
    int n = len / MAX_PAYLOAD_SIZE;
    int m = len %MAX_PAYLOAD_SIZE;
    n += m>0?1:0;
    int i=0;
	for( i=0; i<n; i++)
	{
		int in = i*MAX_PAYLOAD_SIZE;
        bool mark=false;
        uint32_t timestamp=0;
        if(i==n-1)
        {
            int l = m>0?m:MAX_PAYLOAD_SIZE;
            mark = true;
            timestamp = _hz/_framerate;
            std::tie(bufs, buf_len) = mediaPack(_payload_type, (uint8_t*)buf, in, l, true, true);
        }
        else
        {
            _rtpSession.SetDefaultMark(false);
            std::tie(bufs, buf_len) = mediaPack(_payload_type, (uint8_t*)buf, in, MAX_PAYLOAD_SIZE, true, false);
        }
		ret = _rtpSession.SendPacket( bufs, buf_len, _payload_type, mark, timestamp);
		if(ret < 0)
			av_log_error()<<getErrorStr(ret)<<end_log();
	
	}
	
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
