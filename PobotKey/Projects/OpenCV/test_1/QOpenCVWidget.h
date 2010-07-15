#ifndef QOPENCVWIDGET_H
#define QOPENCVWIDGET_H

#include <opencv/cv.h>
#include <QPixmap>
#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>
#include <QImage>

class QOpenCVWidget : public QLabel {
private:
    //QLabel *imagelabel;
    //QVBoxLayout *layout;
    QImage image;

public:
    QOpenCVWidget();
    virtual ~QOpenCVWidget(void) {};
    void putImage(IplImage *);
};

#endif
