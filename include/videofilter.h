#ifndef VIDEOFILTER_H
#define VIDEOFILTER_H

#ifdef __cplusplus
#define __STDC_CONSTANT_MACROS
#ifdef _STDINT_H
#undef _STDINT_H
#endif
# include "stdint.h"
#endif


#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

extern "C"{
#include <libavfilter/avfilter.h>
}

#include <exception>

class VideoFilter
{
public:
    VideoFilter(){}
    VideoFilter(int width, int height, int pix_fmt, AVRational time_base);
    virtual ~VideoFilter();
    virtual bool filter(AVFrame* in_frame, AVFrame** out_frame)=0;

protected:
    AVFilterGraph* _filter_graph=nullptr;
    AVFilterContext* _buffersink_ctx=nullptr;
    AVFilterContext* _buffersrc_ctx=nullptr;
    AVFilterInOut* _outputs=nullptr;
    AVFilterInOut* _inputs=nullptr;

};


class DefaultVidleoFilter : public VideoFilter
{

    // VideoFilter interface
public:
    bool filter(AVFrame *in_frame, AVFrame **out_frame)override;
};


//镜像反转视频
class MirrorVideoFilter : public VideoFilter
{
public:
    MirrorVideoFilter():VideoFilter(){}
    MirrorVideoFilter(int width, int height, int pix_fmt, AVRational time_base)
        :VideoFilter(width, height, pix_fmt, time_base)
    {
        if(!init() )
        {
            avfilter_graph_free(&this->_filter_graph);
            avfilter_inout_free(&this->_inputs);
            avfilter_inout_free(&this->_outputs);
            throw std::exception();
        }
    }

    bool filter(AVFrame *in_frame, AVFrame **out_frame) override;
private:
    bool init();
};

#endif // VIDEOFILTER_H
