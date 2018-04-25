#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <av_decode_filter.h>
#include <av_rtp_source.h>
#include <h264_rtp_packer.h>
#include <av_frame_scale_filter.h>
#include "videodisplaysink.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event);

protected slots:
    void onVideoFrame(const uchar *data, int len, int width, int height);
    void onTimeout();

private:
    Ui::MainWindow *ui;
    QPixmap _pix;
    bool _has_video_data = false;

    QTimer* _timer;

    VideoDisplaySink* _display_sink=nullptr;
    AvFrameScaleFilter* _video_scaler=nullptr;
    AvDecodeFilter* _h264decoder =nullptr;
    AvRtpSource<H264RtpPacker>* _rtpsrc = nullptr;
};

#endif // MAINWINDOW_H
