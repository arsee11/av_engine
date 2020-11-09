//  av_frame_scale_filter.h


#ifndef AV_FRAME_SCALE_FILTER_H
#define AV_FRAME_SCALE_FILTER_H

#include "filter.h"
#include "av_util.h"

class FrameScaler;

class AvFrameScaleFilter:public Filter<AVParam>
{
public:
    static AvFrameScaleFilter* create(PixelFormat f, int width, int height, 
    Transformation<AVParam>* next_filter=nullptr)
    {
        return new AvFrameScaleFilter(f, width, height, next_filter);
    }
    
    void destroy(){delete this; }

private:
    AvFrameScaleFilter(PixelFormat f, int width, int height, 
	Transformation<AVParam>* next_filter=nullptr
	)
    	:Filter<AVParam>(next_filter)
    	,_format(f)
    	,_width(width)
    	,_height(height)
    {
        _param.type = MEDIA_VIDEO;
        _param.codecid = CodecID::CODEC_ID_NONE;
    }

    ~AvFrameScaleFilter();

    bool transform(AVParam* p)override;

    PixelFormat _format, _src_format = PixelFormat::FORMAT_NONE;
    int _width, _height;
    FrameScaler* _scaler = nullptr;
    int _src_width = 0, _src_height = 0;
};
#endif /* AV_FRAME_SCALE_FILTER_H */
