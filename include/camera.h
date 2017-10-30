//  camera.h


#ifndef CAMERA_H
#define CAMERA_H

#include <functional>
#include <string>
#include <thread>
#include <mutex>
#include <list>

extern "C"{
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
}

#include "av_util.h"
#include "source.h"

class Camera :public Source<AVParam>
{
public:
	Camera(Transformation<Param>* ts);
    ~Camera(){
        close();
        _param->release();
    }

    bool open(const char* device, int framerate, int width, int height);
    void close();

private:
	bool transform(AVParam*& p)override;
	AVParam* _param = AVParam::create();


private:
    std::string _device;
    AVFormatContext *_format_ctx=NULL;
    int _video_stream_idx = -1;
    int _width=0;
    int _height=0;
    int _framerate=30;
    bool _is_opened=false;
};

#endif /* CAMERA_H */
