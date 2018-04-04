//  h264_rtp_packer.h


#ifndef H264_RTP_PACKER_H
#define H264_RTP_PACKER_H

#include <cstdint>
#include <tuple>
#include <vector>
#include "rtp_packer.h" 

class H264RtpPacker
{
public:
	H264RtpPacker(int max_pack_size=1400, uint8_t framerate=25)
		:_max_pack_size(max_pack_size)
		,_framerate(framerate)
	{}

	std::vector<RtpPack> pack(uint8_t* data, int len);
    
private:
    	std::tuple<uint8_t*, uint16_t> pack(uint8_t* data, int offset, int len, bool is_fragment, bool is_end);
    	void resetBuf(int len);
    
private:
    uint8_t* _buf=nullptr;
    uint16_t _buf_len=0;
    int _max_pack_size;
    uint8_t _framerate;
    int _hz=90000;
};

#endif /* H264_RTP_PACKER_H */
