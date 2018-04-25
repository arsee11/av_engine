#include "videowidget.h"

#include <QPainter>

VideoWidget::VideoWidget(QWidget *parent) : QWidget(parent)
{

}

void VideoWidget::onVideoFrame(const uchar* data, int len, int width, int height)
{
     //qDebug()<<"onVideoData:"<<len<<","<<width<<"x"<<height;
     QImage img(data, width, height, QImage::Format_RGB888);
     _pix = QPixmap::fromImage(img);
     _has_video_data = true;
     this->update();
}


void VideoWidget::paintEvent(QPaintEvent *event)
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
