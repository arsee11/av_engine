//  H264RTPPacker.h


#ifndef H264_RTP_PACKER_H
#define H264_RTP_PACKER_H

#include <cstdint>
#include <tuple>

class H264RTPPacker
{
public:
    static std::tuple<uint8_t*, uint16_t> pack(uint8_t* data, int offset, int len, bool is_fragment, bool is_end);
    
private:
    static void resetBuf(int len);
    
private:
    static uint8_t* s_buf;
    static uint16_t s_buf_len;
};

#endif /* H264_RTP_PACKER_H */
