///rtp_paker.h

#ifndef RTP_PACKER_H
#define RTP_PACKER_H

///@brief not include rtp header.
class RtpPack
{
public:
	RtpPack(void* data, int len, bool mark, int timestamp)
		:_data(data)
		,_len(len)
		,_mark(mark)
		,_timestamp(timestamp)
	{}

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

	const void* data(){ return _data; }
	uint32_t len(){ return _len; }
	bool mark(){ return _mark; }
	uint32_t timestamp(){ return _timestamp; }

private:
	RtpPack(const RtpPack&);
	RtpPack& operator=(const RtpPack&);

	void* _data=nullptr;
	uint32_t _len=0;
	bool _mark=true;
	uint32_t _timestamp=0;
	
};

#endif /*RTP_PACKER_H*/
