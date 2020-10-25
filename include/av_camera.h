//  av_camera.h


#ifndef AV_CAMERA_H
#define AV_CAMERA_H

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

class AvCamera :public Source<AVParam>
{
public:
	AvCamera(Transformation<Param>* ts);
	AvCamera(int framerate, int width, int height, Transformation<Param>* ts);
	AvCamera(int framerate, int width, int height);
    ~AvCamera(){
        close();
    }

    bool open(const char* device);
    bool open(const char* device, int framerate, int width, int height);
    void close();

	AVParam* get()override;
    CodecID codec() { return _param.codecid;  }
    int width()const{ return _width; }
    int height()const{ return _height; }

private:
    std::string _device;
    AVFormatContext *_format_ctx=NULL;
    int _video_stream_idx = -1;
    int _width=0;
    int _height=0;
    int _framerate=30;
    bool _is_opened=false;
    AVPacket* _pack = nullptr;

};

#endif /* AV_CAMERA_H */
