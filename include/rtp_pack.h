///rtp_pack.h

#ifndef RTP_PACK_H
#define RTP_PACK_H

#include <string.h>

///@brief not include rtp header.
class RtpPack
{
public:
	RtpPack(uint8_t* data, int len, bool mark, int timestamp, bool is_deep_copy)
		:_len(len)
		,_mark(mark)
		,_timestamp(timestamp)
		,_is_deep_copy(is_deep_copy)
	{
		if( data != nullptr && len >0 )
		{
			if(is_deep_copy)
			{
				_data = new uint8_t[len];
				memcpy(_data, data, len);
			}
			else
				_data = data;
		}
	}

	~RtpPack()
	{
		if(_data != nullptr && _is_deep_copy)
			delete[] _data;
	}

	RtpPack& operator=(RtpPack&& rhs){
		if(&rhs == this)
			return *this;
		
		this->_data = rhs._data;
		rhs._data = nullptr;
		this->_len = rhs._len;
		this->_mark = rhs._mark;
		this->_timestamp = rhs._timestamp;
	}

	RtpPack(RtpPack&& other){ *this = std::move(other); }

	const uint8_t* data(){ return _data; }
	uint32_t len(){ return _len; }
	bool mark(){ return _mark; }
	uint32_t timestamp(){ return _timestamp; }

private:
	RtpPack(const RtpPack&);
	RtpPack& operator=(const RtpPack&);

	uint8_t* _data=nullptr;
	uint32_t _len=0;
	bool _mark=true;
	bool _is_deep_copy=true;
	uint32_t _timestamp=0;
	
};

#endif /*RTP_PACK_H*/
