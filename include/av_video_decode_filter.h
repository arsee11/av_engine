///av_video_decode_filter.h

#ifndef AV_VIDEO_DECODE_FILTER_H
#define AV_VIDEO_DECODE_FILTER_H

#include "av_util.h"
#include "filter.h"
#include "codec_specify.h"

class AvVideoDecodeFilter:public Filter<AVParam>
{
public:
    static AvVideoDecodeFilter* create(CodecID cid=CODEC_ID_NONE
        ,Transformation<Param>* next_filter = nullptr
    )
    {
        return new AvVideoDecodeFilter(cid, next_filter);
    }

    void destroy(){delete this; }

    bool open(const CodecInfo& ci);
    void close();

private:
    AvVideoDecodeFilter(CodecID cid, Transformation<Param>* next_filter = nullptr)
        :Filter<AVParam>(next_filter)
        ,_codec_id(cid)
    {
    }
    ~AvVideoDecodeFilter(){
        close();
    }

    bool transform(AVParam* p)override;
    bool open(CodecID cid, int w, int h);
private:

    CodecID _codec_id;
    AVCodecContext* _codec_ctx = nullptr;
    AVPacket* _packet=nullptr;
    AVFrame* _frame = nullptr;
    uint8_t* _buf=nullptr;
    int _buf_size = 0;
};
#endif/*AV_VIDEO_DECODE_FILTER_H*/
