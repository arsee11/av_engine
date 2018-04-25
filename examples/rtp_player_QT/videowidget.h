#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>

class VideoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoWidget(QWidget *parent = nullptr);


public slots:
    ///RGB888
    void onVideoFrame(const uchar *data, int len, int width, int height);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QPixmap _pix;
    bool _has_video_data = false;
};

#endif // VIDEOWIDGET_H
