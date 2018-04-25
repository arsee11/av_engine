#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <qdebug.h>
#include <QTimer>
#include <QThread>

#include "threading/thread.h"
#include "threading/executable_queue.h"
#include "threading/dispatcher.h"

using namespace arsee;

static ExecutableQueue gexec_queue;
static Thread<ExecutableQueue> gthread(&gexec_queue);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _display_sink = new VideoDisplaySink();
    _video_scaler = AvFrameScaleFilter::create(PixelFormat::FORMAT_RGB24, 320, 240, _display_sink);
    _h264decoder = AvDecodeFilter::create(CodecID::H264, _video_scaler);
    _rtpsrc = AvRtpSource<H264RtpPacker>::create(_h264decoder, 8002, H264RtpPacker(25));

    connect(_display_sink, &VideoDisplaySink::recvVideoFrame, this, &MainWindow::onVideoFrame);
    //_timer = new QTimer(this);
    //connect(_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    //_timer->start(1);

    dispatch_asyn(&gexec_queue, &MainWindow::onTimeout, this);
 }

MainWindow::~MainWindow()
{
    gthread.stop();
    delete ui;
}

void MainWindow::onVideoFrame(const uchar* data, int len, int width, int height)
{
     //qDebug()<<"onVideoData:"<<len<<","<<width<<"x"<<height;
     QImage img(data, width, height, QImage::Format_RGB888);
     _pix = QPixmap::fromImage(img);
     _has_video_data = true;
     this->update();
}

void MainWindow::onTimeout()
{
    _rtpsrc->read();
    QThread::msleep(10);
    dispatch_asyn(&gexec_queue, &MainWindow::onTimeout, this);
}


void MainWindow::paintEvent(QPaintEvent *event)
{
     Q_UNUSED(event);

    QPainter painter(this);
    if(_has_video_data)
    {
        painter.drawPixmap(this->rect(), _pix);
        _has_video_data = false;
    }
    else
    {
          painter.fillRect(this->rect(), QColor(30,30,30));
    }
}


