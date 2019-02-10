#ifndef AUDIOPLAYSINK_H
#define AUDIOPLAYSINK_H

#include <sink.h>
#include <av_util.h>
#include <flexible_buffer.h>
#include <QObject>
#include <QAudioOutput>
#include <QTimer>
#include <QBuffer>

class QIODevice;

class AudioPlaySink : public QObject, public Sink<AVParam>
{
    Q_OBJECT

public:
    AudioPlaySink(short sample_rate, short channels, short smaple_size);

    // Transformation interface
public:
    void put(Param *p)override;

protected slots:
    void onTimeOut();

private:
    QAudioOutput* _audio_out=nullptr;
    QIODevice* _audio_io=nullptr;
    QTimer* _timer;
    QBuffer* _buffer;
    FlexibleBuffer<uint8_t> _buf;
};

#endif // AUDIOPLAYSINK_H
