///av_video_decode_filter_nv.h

#ifndef AV_VIDEO_DECODE_FILTER_NV_H
#define AV_VIDEO_DECODE_FILTER_NV_H

#include "av_util.h"
#include "filter.h"
#include "codec_specify.h"
#include <NvDecoder/NvDecoder.h>

class AvVideoDecodeFilterNv:public Filter<AVParam>
{
public:
    static AvVideoDecodeFilterNv* create(CodecID cid=CODEC_ID_NONE
        ,Transformation<Param>* next_filter = nullptr
    )
    {
        return new AvVideoDecodeFilterNv(cid, next_filter);
    }

    void destroy(){delete this; }

    bool open(const CodecInfo& ci);
    void close();

private:
    AvVideoDecodeFilterNv(CodecID cid, Transformation<Param>* next_filter = nullptr)
        :Filter<AVParam>(next_filter)
        ,_codec_id(cid)
    {
    }
    ~AvVideoDecodeFilterNv(){
        close();
    }

    bool transform(AVParam* p)override;
    bool open(CodecID cid, int w, int h);

private:
    CodecID _codec_id;
    uint8_t* _buf=nullptr;
    int _buf_size = 0;
    std::shared_ptr<NvDecoder> _nvdecoder;
};
#endif/*AV_VIDEO_DECODE_FILTER_NV_H*/
