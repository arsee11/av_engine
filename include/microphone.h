//  microphone.h


#ifndef MICROPHONE_H
#define MICROPHONE_H

#include <stdio.h>
#include <thread>

#ifdef __cplusplus
extern "C"
{
#endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#include <libavutil/avutil.h>
#include <libavutil/error.h>

#ifdef __cplusplus
}
#endif

#include "source.h"
#include "av_util.h"

class Microphone :public Source<AVParam>
{
public:
	Microphone(Transformation<Param>* ts);
	~Microphone() {
		_param->release();
	}

    bool open(const char* dev, int sample_rate, int sample_size);
    void close();
    

private:
	bool transform(AVParam*& p)override;
	AVParam* _param = AVParam::create();

private:
    AVFormatContext* _format_ctx = NULL;
    AVCodecContext* _codec_ctx = NULL;
    int _stream_idx;

};

#endif /* MICROPHONE_H */
