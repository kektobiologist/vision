#ifndef MYQLABEL_H
#define MYQLABEL_H

#include <QLabel>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDebug>
#include <QEvent>
#include "calibrator.h"
#include "lineinterpolate.h"
class YUVLabel : public QLabel
{
    Q_OBJECT
public:
    explicit YUVLabel(QWidget *parent = 0);
    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void wheelEvent(QWheelEvent * ev);
    void leaveEvent(QEvent *);
    int x, y;
    Qt::MouseButton mb;
    int delta, ticks;
//    Calibrator cb;
signals:
    void mouseMoveYUV();
    void mousePressYUV(int x, int y);
    void mouseLeaveYUV();
    void mouseRightPressYUV(int x, int y);
    void mouseScrollYUV(int ticks);
public slots:

private:
    LineInterpolate *li;
    
};

#endif // MYQLABEL_H
