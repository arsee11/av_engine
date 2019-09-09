// pcma_rtp_packer.h


#ifndef PCMA_RTP_PACKER_H
#define PCMA_RTP_PACKER_H

#include <cstdint>
#include <vector>
#include "rtp_pack.h"
#include <../third_party/include/flexible_buffer.h>

class PcmaRtpPacker 
{
public: 
	///@frame_ms every pack len (millseconds)
	PcmaRtpPacker(int frame_ms=10, uint8_t channels=1)
		:_frame_ms(frame_ms)
		,_channels(channels)
		,_inbuf(640)
		,_retbuf(640)
		,_outbuf(640)
	{}

	///@param data raw pcma stream
	///@param len bytes of data
    	std::vector<RtpPack> pack(uint8_t* data, int len);

	///@param p RtpPack instance
	///@return 1st raw pcma datal, 2nd data size in bytes
	std::tuple<uint8_t*, int> depack(RtpPack&& p);

	int payload_type()const{ return 8; }
	double timestamp_unit()const;

private:
	int _frame_ms;
	int _channels;
    	arsee::FlexibleBuffer<uint8_t> _outbuf;
    	arsee::FlexibleBuffer<uint8_t> _inbuf, _retbuf;//double recv buf
};

#endif /* PCMA_RTP_PACKER_H*/
