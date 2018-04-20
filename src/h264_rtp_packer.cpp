//  h264_rtp_packer.cpp


#include "h264_rtp_packer.h"
#include <string.h>
#include "av_log.h"

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
    	uint32_t tmp = *((uint32_t*)data);
    	if( tmp == start_code4)
    	    	return std::make_tuple(data + 4, len-4);
    	
	uint8_t head3[4]={data[0], data[1], data[2], 0x0};
	tmp = *((uint32_t*)head3);
    	if(tmp == start_code3)
    	    	return std::make_tuple(data + 3, len-3);
    	
    	return std::make_tuple(data, len);
}

uint8_t is_start_code(const uint8_t* data)
{
    	uint32_t tmp = *((const uint32_t*)data);
    	if( tmp == start_code4)
    		return 4;

	uint8_t head3[4]={data[0], data[1], data[2], 0x0};
	tmp = *((const uint32_t*)head3);
    	if(tmp == start_code3)
		return 3;

	return 0;
}


struct H264Nalu
{
	H264Nalu(const uint8_t* d, int l, uint8_t ls)
		:buf(d)
		,len(l)
		,len_start_code(ls)
	{}

	const uint8_t* buf=nullptr;
	int len=0;
	uint8_t len_start_code=0;
};

std::vector<H264Nalu> getNalu(const uint8_t* data, int len)
{
	std::vector<H264Nalu> nalus;
	int idx=-1;
	uint8_t len_start_code_1=0;
	for(int i=0; i<len; )
	{
		uint8_t len_start_code_2=0;
		if( (len_start_code_2=is_start_code(data+i)) > 0 )
		{
			//NALU_HEADER* nh = (NALU_HEADER*)(data+i+len_start_code_2);
		    	//av_log_info()<<"nalu.type="<<nh->TYPE<<end_log();        
			if(idx == -1)
			{
				len_start_code_1 = len_start_code_2;
				idx=i;
			}
			else
			{
				H264Nalu nalu(data+idx, i-idx, len_start_code_1);
				nalus.push_back(nalu); 
				idx=i;
			}

			len_start_code_1 = len_start_code_2;
			i += len_start_code_2;
		}
		else 
			i++;
	}


	H264Nalu nalu(data+idx, len-idx, len_start_code_1);
	nalus.push_back(nalu); 
	return nalus;
}


static FU_INDICATOR fui;
static NALU_HEADER naluh;


double H264RtpPacker::timestamp_unit()const
{
	return (double)1/_hz;
}

	
std::vector<RtpPack> H264RtpPacker::pack(const uint8_t* data, int len)
{
	
	uint32_t timestamp = 1;//PTS++ no B-frame
	std::vector<RtpPack> pkgs;

	std::vector<H264Nalu> nalus = getNalu(data, len);
	for(auto& in : nalus)
	{
		const uint8_t* tmp = in.buf+in.len_start_code;
		int tmp_len = in.len-in.len_start_code;

    		int ret = 0;
    		uint8_t* buf=nullptr;
    		uint16_t buf_len=0;
    		if(tmp_len <= _max_pack_size)
    		{
    		    	std::tie(buf, buf_len)= pack(tmp, 0, tmp_len, false, true);
    		    	pkgs.push_back( RtpPack(buf, buf_len, true, timestamp, true) );
    		    
			continue;
    		}
    		
    		int n = tmp_len / _max_pack_size;
    		int m = tmp_len %_max_pack_size;
    		n += m>0?1:0;
    		int i=0;
		for( i=0; i<n; i++)
		{
			int in = i*_max_pack_size;
        		if(i==n-1)
        		{
        		    	int l = m>0?m:_max_pack_size;
        		    	std::tie(buf, buf_len) = pack(tmp, in, l, true, true);
        			pkgs.push_back( RtpPack(buf, buf_len, true, timestamp, true) );
        		}
        		else
        		{
        		    	std::tie(buf, buf_len) = pack(tmp, in, _max_pack_size, true, false);
        			pkgs.push_back( RtpPack(buf, buf_len, false, 0, true) );
        		}
		
		}
	}
	
	return std::move(pkgs);

}

static FILE* fp = fopen("./send.h264", "wb");
std::tuple<uint8_t*, uint16_t> H264RtpPacker::pack(const uint8_t* data, int offset, int len, bool is_fragment, bool is_end)
{
    uint8_t head[2];
    data += offset;

    if(is_fragment)
    {
        if(offset == 0)//the first fragment
        {
            naluh = *( (NALU_HEADER*)data );
            fui.TYPE = 28;
            fui.NRI = naluh.NRI;
            fui.F = naluh.F;
            
            FU_HEADER fuh{0,0,0,0};
            fuh.S = 1;
            fuh.TYPE = naluh.TYPE;
	    //av_log_info()<<"nalu.type="<<naluh.TYPE<<end_log();        

	    _outbuf.clear();
            head[0] = *((uint8_t*)&fui);
            head[1] = *((uint8_t*)&fuh);
	    _outbuf.push(head, 2);
            _outbuf.push(data+1, len-1);
            
        }
        else if(is_end) //the last fragment
        {
            FU_HEADER fuh{0,0,0,0};
            fuh.E = 1;
            fuh.TYPE = naluh.TYPE;
            
	    _outbuf.clear();
            head[0] = *((uint8_t*)&fui);
            head[1] = *((uint8_t*)&fuh);
	    _outbuf.push(head, 2);
            _outbuf.push(data, len);
            
        }
        else //the middle fragments
        {
            FU_HEADER fuh{0,0,0,0};
            fuh.TYPE = naluh.TYPE;
            
	    _outbuf.clear();
            head[0] = *((uint8_t*)&fui);
            head[1] = *((uint8_t*)&fuh);
	    _outbuf.push(head, 2);
            _outbuf.push(data, len);
        }
    }
    else
    {
        //NALU_HEADER nh = *( (NALU_HEADER*)tmp );
	//av_log_info()<<"nalu.type="<<nh.TYPE<<end_log();        
	_outbuf.clear();
        _outbuf.push(data, len);
    }
    
    fwrite(&start_code4, sizeof(start_code4), 1, fp);
    fwrite(_outbuf.begin(), _outbuf.size(), 1, fp);
    fflush(fp);
    return _outbuf.head(_outbuf.size());
}
    

std::tuple<uint8_t*, int> H264RtpPacker::depack(RtpPack&& p)
{
	const uint8_t* data = p.data();
	int len = p.len();

	FU_INDICATOR* fu = (FU_INDICATOR*)data; 
	//sigle nalu
	if(fu->TYPE>=1 && fu->TYPE <=23 )
	{
		_inbuf.clear();
		_inbuf.push((uint8_t*)&start_code4, 4);
		_inbuf.push(data, len);
		return _inbuf.head(_inbuf.size());
	}
	//fragment nalu
	if(fu->TYPE== 28 )
	{
		//first fragment
		FU_HEADER* fuh = (FU_HEADER*)(data+1);
		av_log_info()<<"S="<<fuh->S<<", E="<<fuh->E<<end_log();
		if(fuh->S == 1)
		{
			NALU_HEADER nh;
			nh.F = fu->F;
			nh.NRI = fu->NRI;
			nh.TYPE = fuh->TYPE;

			_inbuf.clear();
			_inbuf.push((uint8_t*)&start_code4, 4);
			_inbuf.push((uint8_t*)&nh, 1);
			_inbuf.push(data+2, len-2);
			_is_S_recv=true;
		}
		else if(_is_S_recv)
		{
			_inbuf.push(data+2, len-2);
			//last framgent
			if(fuh->E == 1 && p.mark())
			{
				_is_S_recv=false;
				return _inbuf.head(_inbuf.size() );
			}
		}
		else
			av_log_error()<<"drop a invalid packet"<<end_log();
	}
	else
	{
		av_log_error()<<"H264 depack: nalu type:"<<fu->TYPE<<" not surpported."<<end_log();
	}

	return std::make_tuple(nullptr, 0);
}
