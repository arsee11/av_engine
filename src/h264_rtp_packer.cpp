//  h264_rtp_packer.cpp


#include "h264_rtp_packer.h"
#include <string.h>
#include <stdio.h>

/*
 +---------------+
 |0|1|2|3|4|5|6|7|
 +-+-+-+-+-+-+-+-+
 |F|NRI|  Type   |
 +---------------+
 */
typedef struct
{
    unsigned char TYPE:5;
    unsigned char NRI:2;
    unsigned char F:1;
} NALU_HEADER;


/*
 +---------------+
 |0|1|2|3|4|5|6|7|
 +-+-+-+-+-+-+-+-+
 |F|NRI|  Type   |
 +---------------+
 */
typedef NALU_HEADER FU_INDICATOR;

/*
 +---------------+
 |0|1|2|3|4|5|6|7|
 +-+-+-+-+-+-+-+-+
 |S|E|R|  Type   |
 +---------------+
 */
typedef struct
{
    unsigned char TYPE:5;
    unsigned char R:1; //Reserved
    unsigned char E:1; //End
    unsigned char S:1; //Start
} FU_HEADER;


static const uint32_t start_code3 = 0x00010000;
static const uint32_t start_code4 = 0x01000000;

std::tuple<uint8_t*, uint16_t> trim(uint8_t* data, uint16_t len)
{
    int tmp = *((int*)data);
    if( tmp == start_code3)
        return std::make_tuple(data + 3, len-3);
    
    if(tmp == start_code4)
        return std::make_tuple(data + 4, len-4);
    
    return std::make_tuple(data, len);
}

static FU_INDICATOR fui;
static NALU_HEADER naluh;


void H264RtpPacker::resetBuf(int len)
{
    if(_buf_len < len)
    {
        if(_buf!=nullptr)
            delete[] _buf;
        
        _buf = new uint8_t[len];
    }
       
    _buf_len = len;
}

std::vector<RtpPack> H264RtpPacker::pack(uint8_t* data, int len)
{
	uint32_t timestamp = _hz/_framerate;
	std::vector<RtpPack> pkgs;

    	int ret = 0;
    	uint8_t* bufs=nullptr;
    	uint16_t buf_len=0;
    	if(len <= _max_pack_size)
    	{
    	    	std::tie(bufs, buf_len)= pack((uint8_t*)data, 0, len, false, true);
    	    	pkgs.push_back( RtpPack(bufs, buf_len, true, timestamp) );
    	    
    	    	return std::move(pkgs);
    	}
    	
    	int n = len / _max_pack_size;
    	int m = len %_max_pack_size;
    	n += m>0?1:0;
    	int i=0;
	for( i=0; i<n; i++)
	{
		int in = i*_max_pack_size;
        	if(i==n-1)
        	{
        	    	int l = m>0?m:_max_pack_size;
        	    	std::tie(bufs, buf_len) = pack((uint8_t*)data, in, l, true, true);
        		pkgs.push_back( RtpPack(bufs, buf_len, true, timestamp) );
        	}
        	else
        	{
        	    	std::tie(bufs, buf_len) = pack((uint8_t*)data, in, _max_pack_size, true, false);
        		pkgs.push_back( RtpPack(bufs, buf_len, false, 0) );
        	}
	
	}
	
	return std::move(pkgs);

}

FILE* fp = fopen("./test.h264", "wb");
std::tuple<uint8_t*, uint16_t> H264RtpPacker::pack(uint8_t* data, int offset, int len, bool is_fragment, bool is_end)
{
    uint8_t* tmp=nullptr;
    int16_t tmp_len=0;
    std::tie(tmp, tmp_len)= trim(data+offset, len);
    
    if(is_fragment)
    {
        if(offset == 0)//the first fragment
        {
            naluh = *( (NALU_HEADER*)tmp );
            fui.TYPE = 28;
            fui.NRI = naluh.NRI;
            fui.F = naluh.F;
            
            FU_HEADER fuh{0,0,0,0};
            fuh.S = 1;
            fuh.TYPE = naluh.TYPE;
            
            resetBuf(tmp_len+1);
            _buf[0] = *((uint8_t*)&fui);
            _buf[1] = *((uint8_t*)&fuh);
            memcpy(_buf+2, tmp+1, tmp_len-1);
            
            NALU_HEADER nh;
            nh.TYPE = fuh.TYPE;
            nh.NRI = fui.NRI;
            nh.F = fui.F;
            
            fwrite(&start_code4, sizeof(start_code4), 1, fp);
            fwrite(&nh, 1, 1, fp);
            fwrite(_buf+2, tmp_len-1, 1, fp);
            fflush(fp);
        }
        else if(is_end) //the last fragment
        {
            FU_HEADER fuh{0,0,0,0};
            fuh.E = 1;
            fuh.TYPE = naluh.TYPE;
            
            resetBuf(tmp_len+2);
            _buf[0] = *((uint8_t*)&fui);
            _buf[1] = *((uint8_t*)&fuh);
            memcpy(_buf+2, tmp, tmp_len);
            
            fwrite(tmp, tmp_len, 1, fp);
            fflush(fp);
        }
        else //the middle fragments
        {
            FU_HEADER fuh{0,0,0,0};
            fuh.TYPE = naluh.TYPE;
            
            resetBuf(tmp_len+2);
            _buf[0] = *((uint8_t*)&fui);
            _buf[1] = *((uint8_t*)&fuh);
            memcpy(_buf+2, tmp, tmp_len);
            
            fwrite(tmp, tmp_len, 1, fp);
            fflush(fp);
        }
    }
    else
    {
        NALU_HEADER* nh = (NALU_HEADER*)tmp;
        resetBuf(tmp_len);
        memcpy(_buf, tmp, tmp_len);
        
        fwrite(&start_code4, sizeof(start_code4), 1, fp);
        fwrite(tmp, tmp_len, 1, fp);
        fflush(fp);
    }
    
    return std::make_tuple(_buf, _buf_len);
}
    

