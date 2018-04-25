#ifndef VIDEODISPLAY_H
#define VIDEODISPLAY_H

#include <QObject>
#include <sink.h>
#include <av_util.h>

class VideoDisplaySink:  public QObject, public Sink<AVParam>
{
    Q_OBJECT

public:
    VideoDisplaySink()
    {}

signals:
    void recvVideoFrame(uchar* data, int len, int w, int h);

    // Transformation interface
public:
    void put(Param *p);
};

#endif // VIDEODISPLAY_H
