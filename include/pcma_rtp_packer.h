// pcma_rtp_packer.h


#ifndef PCMA_RTP_PACKER_H
#define PCMA_RTP_PACKER_H

#include <cstdint>
#include <vector>
#include "rtp_pack.h"

class PcmaRtpPacker 
{
public: 
	///@frame_ms every pack len (millseconds)
	PcmaRtpPacker(int frame_ms=10, uint8_t channels=1)
		:_frame_ms(frame_ms)
		,_channels(channels)
	{}

    	std::vector<RtpPack> pack(uint8_t* data, int len);
	int payload_type()const{ return 8; }
	double timestamp_unit()const;

private:
	int _frame_ms;
	int _channels;
};

#endif /* PCMA_RTP_PACKER_H*/
