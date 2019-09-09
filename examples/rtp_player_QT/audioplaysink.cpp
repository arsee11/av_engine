#include "audioplaysink.h"
#include <QAudioInput>
#include <QDebug>

AudioPlaySink::AudioPlaySink(short sr, short channels, short smaple_size)
    :_buf(640)
{
    QAudioFormat af;
    af.setSampleRate(sr);
    af.setChannelCount(channels);
    af.setSampleSize(smaple_size);
    af.setCodec("audio/pcm");
    af.setByteOrder(QAudioFormat::LittleEndian);
    QAudioDeviceInfo info = QAudioDeviceInfo::defaultOutputDevice();
    if (!info.isFormatSupported(af)) {
        qDebug()<<"default format not supported try to use nearest";
        af = info.nearestFormat(af);
    }
    _audio_out = new QAudioOutput(af, this);
   // _audio_out->setVolume(0.8);
    _audio_io = _audio_out->start();

    _buffer = new QBuffer(this);
    _buffer->open(QIODevice::ReadWrite);

    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, this, &AudioPlaySink::onTimeOut);
    _timer->start(10);
}

//FILE* fp=fopen("recv.pcm", "wb");
void AudioPlaySink::onTimeOut()
{
    if(_audio_out!=nullptr
       &&_audio_out->state()!= QAudio::StoppedState
       &&_audio_out->state()!= QAudio::SuspendedState)
    {
        //if ( _buffer->size() >= _audio_out->periodSize())
        if ( _buf.size() >= _audio_out->periodSize())
        {
            //_audio_io->write(_buffer->data(), _buffer->size());
            //_buffer->reset();
            _audio_io->write( (const char*)_buf.begin(), _audio_out->periodSize());
            _buf.consume(_audio_out->periodSize());
            int s = _buf.size();
            qDebug()<<s;
        }
        else
        {
            qDebug()<<"data len to small len="<<_buffer->size()<<", periodSize="<<_audio_out->periodSize();
        }
    }
}

void AudioPlaySink::put(Param *p)
{
    _buf.push(p->getData(), p->len);
    //fwrite(p->getData(), p->len, 1, fp);
    //fflush(fp);
    //int rs =  _buffer->write((const char*)p->getData(), p->len);
    //qDebug()<<"_buffer->write:"<<rs<<"_buffer size:"<<_buffer->size();
}
