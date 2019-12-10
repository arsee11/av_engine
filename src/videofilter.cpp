#include "videofilter.h"

extern "C"{
#include <libavfilter/buffersrc.h>
#include <libavfilter/buffersink.h>
#include <libavutil/opt.h>
}

VideoFilter::VideoFilter(int width, int height, int pix_fmt, AVRational time_base)
{
    _filter_graph = avfilter_graph_alloc();
    const AVFilter* buffer_src = avfilter_get_by_name("buffer");
    const AVFilter* buffer_sink = avfilter_get_by_name("buffersink");
    char args[512]={0};
    snprintf(args, sizeof(args),
                "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d",
                width, height, pix_fmt,time_base.num, time_base.den);

    if( avfilter_graph_create_filter(&_buffersrc_ctx, buffer_src, "in", args, NULL, _filter_graph) < 0 )
    {
        avfilter_graph_free(&_filter_graph);
        throw std::exception();
    }
    if( avfilter_graph_create_filter(&_buffersink_ctx, buffer_sink, "out", NULL, NULL, _filter_graph) <0 )
    {
        avfilter_graph_free(&_filter_graph);
        throw std::exception();
    }

    int pix_fmts[] = { pix_fmt, AV_PIX_FMT_NONE };
    if( av_opt_set_int_list(_buffersink_ctx, "pix_fmts", pix_fmts, AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN) < 0)
    {
        avfilter_graph_free(&_filter_graph);
        throw std::exception();
    }
    _outputs = avfilter_inout_alloc();
    _inputs  = avfilter_inout_alloc();
    _outputs->name = av_strdup("in");
    _outputs->pad_idx = 0;
    _outputs->filter_ctx = _buffersrc_ctx;
    _outputs->next = NULL;

    _inputs->name = av_strdup("out");
    _inputs->pad_idx = 0;
    _inputs->filter_ctx = _buffersink_ctx;
    _inputs->next = NULL;
}

VideoFilter::~VideoFilter()
{
    if(_filter_graph != nullptr)
        avfilter_graph_free(&_filter_graph);

    if(_inputs != nullptr)
        avfilter_inout_free(&_inputs);

    if(_outputs != nullptr)
        avfilter_inout_free(&_outputs);
}

bool MirrorVideoFilter::filter(AVFrame *in_frame, AVFrame **out_frame)
{
    if(this->_buffersrc_ctx==nullptr)
    {
        *out_frame = in_frame;
        return true;
    }
    *out_frame = av_frame_alloc();
    if( av_buffersrc_add_frame_flags(this->_buffersrc_ctx, in_frame, AV_BUFFERSRC_FLAG_KEEP_REF) < 0 )
        return false;

    while(true)
    {
        int ret;
        ret = av_buffersink_get_frame(this->_buffersink_ctx, *out_frame);
        if( ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            break;

        if(ret < 0)
            return false;
    }
    av_frame_unref(in_frame);

    return true;
}

bool MirrorVideoFilter::init()
{
    if( avfilter_graph_parse_ptr(this->_filter_graph, "hflip", &this->_inputs, &this->_outputs, NULL)< 0)
        return false;

    if( avfilter_graph_config(this->_filter_graph, NULL)<0)
        return false;

    return true;
}

bool DefaultVidleoFilter::filter(AVFrame *in_frame, AVFrame **out_frame)
{
    //*out_frame = av_frame_clone(in_frame);
    //*out_frame = av_frame_alloc();
    //int rc = av_frame_ref(*out_frame, in_frame);
    //char ebuf[257]={0};
    //av_make_error_string(ebuf, 256, rc);
    *out_frame = in_frame;
    return true;
}
