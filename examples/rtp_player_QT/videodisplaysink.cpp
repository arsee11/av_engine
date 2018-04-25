#include "videodisplaysink.h"


void VideoDisplaySink::put(Param *p)
{
    emit recvVideoFrame(p->getData(), p->len, p->w, p->h);
}
