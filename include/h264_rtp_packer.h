//  h264_rtp_packer.h


#ifndef H264_RTP_PACKER_H
#define H264_RTP_PACKER_H

#include <cstdint>
#include <tuple>
#include <vector>
#include "rtp_pack.h" 
#include "flexible_buffer.h" 

class H264RtpPacker
{
public:
	H264RtpPacker(uint8_t framerate=25, int payload_type=96, int max_pack_size=1400)
		:_framerate(framerate)
		,_payload_type(payload_type)
		,_max_pack_size(max_pack_size)
		,_inbuf(65536)
		,_outbuf(max_pack_size+2)
	{}

	///@param data one H264 ES 
	///@param len bytes of data
	std::vector<RtpPack> pack(const uint8_t* data, int len);

	///@param p RtpPack instance
	///@return a H264 frame, frame size
	std::tuple<uint8_t*, int> depack(RtpPack&& p);

	int payload_type()const{ return _payload_type; }
	double timestamp_unit()const;
    
private:
    	std::tuple<uint8_t*, uint16_t> pack(const uint8_t* data, int offset, int len, bool is_fragment, bool is_end);
    
private:
    	arsee::FlexibleBuffer<uint8_t> _outbuf;
    	arsee::FlexibleBuffer<uint8_t> _inbuf;
    	int _max_pack_size;
    	int _payload_type;
    	uint8_t _framerate;
    	int _hz=90000;
    	bool _is_S_recv=false;
};

#endif /* H264_RTP_PACKER_H */
