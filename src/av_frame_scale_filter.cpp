﻿//  av_frame_scale_filter.cpp


#include "av_frame_scale_filter.h"
#include "frame_scaler.h"
#include <tuple>

AvFrameScaleFilter::~AvFrameScaleFilter()
{
    if(_scaler != nullptr)
        delete _scaler;
}

bool AvFrameScaleFilter::transform(AVParam* p)
{
    _param.fps = p->fps;
    if(p->format != _format || p->w != _width || p->h != _height)
    {
    	if (p->w != _src_width || p->h != _src_height || p->format != _src_format)
    	{
            if (_scaler != nullptr)
                delete _scaler;

            _scaler = new FrameScaler(p->w, p->h, (PixelFormat)p->format, _width, _height, _format);
            _src_width = p->w;
            _src_height = p->h;
            _src_format = (PixelFormat)p->format;
    	}

    	_scaler->scale(p->data_ptr(), p->size());
    	_param.data(_scaler->dst_frame_data(), _scaler->dst_frame_size());
    	_param.format = _format;
        _param.w = _width;
        _param.h = _height;
    }
    else
    {
        _param.format = p->format;
        _param.w = p->w;
        _param.h = p->h;
        _param.data(p->data_ptr(), p->size());
    }
    
    return true;
}
