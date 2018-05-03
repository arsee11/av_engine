// pcma_rtp_packer.cpp


#include "pcma_rtp_packer.h"
#include "av_log.h"

const float sample_rate = 8000;
const float bit_rate = 8000*8; //bps

std::vector<RtpPack> PcmaRtpPacker::pack(uint8_t* data, int len)
{
	int plen = (bit_rate*((float)_frame_ms/1000) )/8 * _channels; //Bytes 
	std::vector<RtpPack> pkgs;
        uint32_t timestamp= sample_rate/(1000/_frame_ms);
	//av_log_info()<<"timestamp:"<<timestamp<<end_log();
        bool mark=true;

	if(len < plen)
    	{
	///ToDo buffer the datas unit len > plen
		return std::move(pkgs);
   	}     
    
    
    	int n = len / plen;
    	int m = len % plen;
	for( int i=0; i<n; i++)
	{
		int offset = i*plen;
		pkgs.push_back( RtpPack(data+offset, plen, mark, timestamp, true) );
	
	}
	if(m > 0)
	{
	///ToDo buffer the datas  
	}
	
	return std::move(pkgs);
}

std::tuple<uint8_t*, int> PcmaRtpPacker::depack(RtpPack&& p)
{
	int plen = (bit_rate*((float)_frame_ms/1000) )/8 * _channels; //Bytes 
	_inbuf.push(p.data(), p.len());
	int n = _inbuf.size()/plen;
	if(n>0)
	{
		_retbuf.clear();
		_retbuf.push( _inbuf.begin(), n*plen);;
		_inbuf.consume(n*plen);
		return std::make_tuple(_retbuf.begin(), n*plen);
	}
	else
		return std::make_tuple(nullptr, 0); 
}

double PcmaRtpPacker::timestamp_unit()const
{
	return 1.0/sample_rate;
}
