//av_rtp_source.h

#ifndef AV_RTP_SOURCE_H
#define AV_RTP_SOURCE_H

#include <assert.h>

#include "av_exception.h"
#include "av_util.h"
#include "rtp_wrapper.h"
#include "source.h"
#include "av_log.h"
#include "rtp_pack.h"


class AvRtpSourceException: public AvException
{
public:
	AvRtpSourceException(const char* whats)
		:AvException(whats)
	{
	}

	AvRtpSourceException(int errorCode)
		:AvException(errorCode)
	{	
	}
};

//static FILE* f = fopen("recv.h264", "wb");

///@brief receive rtp packets form network.
template<class RtpDepacker>
class AvRtpSource : public Source<AVParam>
{
	typedef AVParam param_t;
	typedef RtpWrapper::rtp_wrapper_ptr_t rtp_wrapper_ptr_t;

public:
	static AvRtpSource* create(Transformation<Param>* ts, uint16_t localPort, const RtpDepacker& dpk)
	{
		return new AvRtpSource(ts, localPort, dpk);
	}

	rtp_wrapper_ptr_t getRtpWrapper() const { return _rtp; }

	AVParam* get()override {
		uint8_t buf[65536];
		int size=0;
		uint32_t timestamp=0; 
		bool mark=false;
		std::tie(timestamp, mark, size) = _rtp->readPacket(buf, 65536);
		if (size > 0)
		{
			//av_log_info()<<"rtp recv a packet: timestamp="<<timestamp<<",mark="<<mark<<",size="<<size<<end_log();
			uint8_t* frame;
			int fsize;
			std::tie(frame, fsize)=_depacker.depack(RtpPack(buf, size, mark, timestamp, false));
			if(fsize > 0)
			{
				//av_log_info()<<"start code="<<*(uint32_t*)frame<<", frame size="<<fsize<<end_log();
				_param->setData(frame, fsize);
				//fwrite(frame, fsize, 1, f);
				//fflush(f);
                                return _param;
			}
                }

		return nullptr;
	}

private:
	AvRtpSource(Transformation<Param>* ts, uint16_t localPort, const RtpDepacker& dpk)
		:Source(ts)
		,_localPort(localPort)
		,_depacker(dpk)
		,_rtp(nullptr)
	{
		_rtp = rtp_wrapper_ptr_t(new RtpWrapper); 
		_rtp->open(_localPort, dpk.timestamp_unit(), dpk.payload_type());
	}

	AvRtpSource(Transformation<Param>* ts, rtp_wrapper_ptr_t rtp)
		:Source(ts)
		,_localPort(0)
		,_rtp(rtp)
	{
	}

private:
	rtp_wrapper_ptr_t _rtp;
	uint16_t _localPort;
	RtpDepacker _depacker;
	param_t* _param = param_t::create();
};

#endif /*AV_RTP_SOURCE_H*/
