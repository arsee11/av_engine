///rtpheader.h
//
//

#include <cstdint>

struct RtpHeader
{
#ifdef BIG_ENDIAN
	uint8_t V:2; //Version 2.
	uint8_t P:1; //padding.
	uint8_t X:1; //extesion flag.
	uint8_t CC:4;//count of CSRC

	uint8_t M:1; //maker flag.
	uint8_t PT:7;//payload type.
#else //little endian
	uint8_t CC : 4;//count of CSRC
	uint8_t X : 1; //extesion flag.
	uint8_t P : 1; //padding.
	uint8_t V : 2; //Version 2.

	uint8_t PT : 7;//payload type.
	uint8_t M : 1; //maker flag.
#endif
	uint16_t SN;//sequence number.
	uint32_t timestamp;
	uint32_t CCRC;

	RtpHeader()
		:V(2)
		,P(0)
		,X(0)
		,CC(0)
		,M(0)
		,PT(0)
		,SN(0)
		,timestamp(0)
		,CCRC(0)
	{}

};

struct RtpExHeader
{
	uint16_t profile_id;  //user defined used.
	uint16_t size;     //size of 32Byte fields.	
};

#ifdef _MSC_VER
#include <WinSock2.h>

#pragma comment(lib, "Ws2_32")
#endif

#ifdef _LINUX_
#include <arpa/inet.h>
#endif

int main()
{
	RtpHeader h;
	
	uint8_t *buf = (uint8_t*)&h;
	
	h.M=1;
	h.PT=96;
	h.SN=htons(1001);
	h.timestamp=htonl(123456);
	h.CCRC=htonl(22311431);

	RtpHeader *h2 = (RtpHeader*)buf;
	RtpHeader h3 = *(RtpHeader*)buf;

	return 0;
};
