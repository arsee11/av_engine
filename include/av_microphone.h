//  av_AvMicrophone.h


#ifndef AvMicrophone_H
#define AvMicrophone_H

#include <stdio.h>
#include <thread>

extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#include <libavutil/avutil.h>
#include <libavutil/error.h>
}

#include "source.h"
#include "av_util.h"

class AvMicrophone :public Source<AVParam>
{
public:
	AvMicrophone(Transformation<Param>* ts);
	~AvMicrophone() {
		_param->release();
	}

    bool open(const char* dev, int sample_rate, int sample_size);
    void close();
    
	AVParam* get()override;

private:
	AVParam* _param = AVParam::create();

private:
    AVFormatContext* _format_ctx = NULL;
    AVCodecContext* _codec_ctx = NULL;
    int _stream_idx;

};

#endif /* AvMicrophone_H */
