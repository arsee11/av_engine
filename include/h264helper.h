///h264helper.h
//

#include <cstdint>
#include <vector>
#include <flexible_buffer.h>

struct NaluS
{
	uint8_t* data;
	uint32_t size;
};

enum NaluType
{
	UNKNOWN=-1,
	SPS=7,
	PPS=8,
	IDR=5,
	NOTIDR=1
};

class H264Helper
{
public:
	H264Helper(uint32_t max_buf_size)
	:_buf(max_buf_size)
	,_max_buf_size(max_buf_size){}

	static bool isIFrame(uint8_t* data, uint32_t size);
	//return 3 or 4 is sart, otherwise is not start
	static int isStart(const uint8_t* data);
	static std::vector<NaluS> split(uint8_t* data, uint32_t size);
	static NaluType naluType(uint8_t byte){ return (NaluType)(byte & 0x1f); }
	
	void push(const uint8_t* data, uint32_t size);
	void pop(uint32_t size);
	std::vector<NaluS> getFrames(uint32_t& out_total);

public:
	arsee::FlexibleBuffer<uint8_t> _buf;	
	uint32_t _max_buf_size = 1024*512;
};
