///raw_codec.h

#ifndef RAW_CODEC_H
#define RAW_CODEC_H


extern "C"{
	#ifndef AVCODEC_AVCODEC_H
	#include <libavcodec/avcodec.h>
	#endif 
}

class RawCodec
{
public:
	enum{ ID = AV_CODEC_ID_NONE	};
	enum{ HZ=90000,
		PlayloadType = 96
	};

	static bool IsSame(AVCodecID id){ return (AVCodecID)ID == id; }
};



#endif /*RAW_CODEC_H*/