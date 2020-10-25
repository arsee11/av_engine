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
    ~AvMicrophone()
    {
        close();
    }

    bool open(const char* dev, int sr, int sample_size, int channels=2);
    void close();
    
	AVParam* get()override;

private:
    AVFormatContext* _format_ctx = nullptr;
    int _stream_idx;
    AVPacket* _pack = nullptr;


};

#endif /* AvMicrophone_H */
