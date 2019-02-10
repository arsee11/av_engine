///h264helper.cpp
//

#include "h264helper.h"

bool H264Helper::isIFrame(uint8_t* data, uint32_t size)
{
	for(uint32_t i=0; i<size-4; i++)
	{
		if (  (0 == data[i] && 0 == data[i+1] && 0 == data[i+2] && 1 == data[i+3] && ( 0x05 ==(data[i+4]&0x1f)))
		    ||(0 == data[i] && 0 == data[i+1] && 1 == data[i+2] && ( 0x05 ==(data[i+3]&0x1f)))
		)
		{
			return true;
		}
	}

	return false;
}

int H264Helper::isStart(const uint8_t* data)
{
	if( 0 == data[0] && 0 == data[1] && 0 == data[2] && 1 == data[3]) 
		return 4;
	else if(0 == data[0] && 0 == data[1] && 1 == data[2] )
		return 3;;
		
	return 0;	
}

std::vector<NaluS> H264Helper::split(uint8_t* data, uint32_t size)
{
	std::vector<NaluS> nalus;
	int32_t start=-1, end=-1;
	for(uint32_t i=0; i<size-4; )
	{
		int n=0;
		if( (n=isStart(data+i))>0 ) 
		{
			end = i;
			if(start != -1)
			{
				NaluS nalu{data+start, end-start};
				nalus.push_back(nalu);
			}
			start = end;
			i+=n;
		}
		else
			i++;

	}

	NaluS nalu{data+start, size-start};
	nalus.push_back(nalu);
	return nalus;
}



void H264Helper::push(const uint8_t* data, uint32_t size)
{
	if(_buf.size() >= _max_buf_size)
		_buf.consume(_buf.size());

	_buf.push(data, size);
}

void H264Helper::pop(uint32_t size)
{
	_buf.consume(size);
}

std::vector<NaluS> H264Helper::getFrames(uint32_t& out_total)
{
	std::vector<NaluS> nalus;
	uint8_t* data = _buf.begin();
	uint8_t* data_begin=data;
	uint32_t total=0;
	for(int i=3; i<_buf.size()-4;) 
	{
		if( isStart(data+i) )
		{
			uint32_t len = (data+i) - data_begin;;
			NaluS nalu{data_begin, len};
			nalus.push_back(nalu);	
			data_begin = data+i;
			total += len;
			i += 3;
		}
		i++;
	}

	out_total = total;
	return nalus;
}


/*
int main()
{
	uint8_t data[]={
		 0x00,	0x00,	0x00,	0x01,	0x67,	0x4d,	0x00,	0x1f
		,0x95,	0xa8,	0x14,	0x01,	0x6e,	0x9b,	0x80,	0x80
		,0x80,	0x81,	0x00,	0x00,	0x00,	0x01,	0x68,	0xee
		,0x3c,	0x80,	0x00,	0x00,	0x00,	0x01,	0x06,	0xe5
		,0x01,	0x51,	0x80,	0x00,	0x00,	0x00,	0x01,	0x65
		,0xb8,	0x77,	0x8b,	0x78,	0xc4,	0x35,	0xf8,	0x17
		,0x55,	0x66,	0x10,	0xae,	0x39,	0xd5,	0x00,	0x48
		,0x90,	0xa2,	0x55,	0x06,	0x00,	0xb8,	0x02,	0x3d
		,0x3c,	0x7c,	0x11,	0xd0,	0xb5,	0xc1,	0x88,	0xb6
		,0x6d,	0x1b,	0x3c,	0xbf,	0x38,	0x5a,	0x0f,	0xc1
		,0x1b,	0xfb,	0x86,	0x3a,	0x93,	0xd2,	0x5e,	0xf5
		,0x61,	0xaa,	0xa9,	0x0b,	0xb7,	0xb7,	0x3a,	0x3b
		,0xf5,	0x77,	0x3f,	0xd2,	0x98,	0xca,	0x5d,	0xfd
		,0xa4,	0xfa,	0xcf,	0xee,	0xce,	0xfe,	0xd7,	0x3d
		,0x6c,	0xce,	0xfa,	0xf3,	0x57,	0x64,	0xd6,	0x38
		,0xb5,	0xf2,	0xe9,	0x9e,	0x5e,	0xc9,	0x53,	0xc9
	};

	H264Helper h(1024);
	h.push(data, 64);
	uint32_t total=0;
	auto ns = h.getFrames(total);
	NaluS n1 = ns[0];
	NaluS n2 = ns[1];
	NaluS n3 = ns[2];

	h.push(data, 9);
	ns = h.getFrames();

	ns = h.getFrames();

	h.push(data, 9);
	h.push(data, 9);
	ns = h.getFrames();
	
	h.push(data+4, 5);
	ns = h.getFrames();

	return 0;
}

*/
