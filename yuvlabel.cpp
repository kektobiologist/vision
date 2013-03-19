#include "yuvlabel.h"

YUVLabel::YUVLabel(QWidget *parent) :
    QLabel(parent)
{
    li = new LineInterpolate(cvSize(256,256)); //Assuming YUV label has size 256, 256.
}

void YUVLabel::mousePressEvent(QMouseEvent *ev)
{

    mb = ev->button();
    if((ev->button() & Qt::LeftButton) /*&& ISDIFF_COORD(ev->x(), ev->y(), x, y)*/)
    {
        x = ev->x();
        y = ev->y();
        emit mousePressYUV(x,y);
    }
    if((ev->button() & Qt::RightButton))
    {
        emit mouseRightPressYUV(ev->x(), ev->y());
    }
}

void YUVLabel::mouseMoveEvent(QMouseEvent *ev)
{
    int newx = ev->x();
    int newy = ev->y();
    int count=0;
    if(ISDIFF_COORD(newx, newy, x, y))
    {
        if(ev->buttons() & Qt::LeftButton)
        {
            count = li->init(cvPoint(x>>V_BITS,y>>U_BITS), cvPoint(newx>>V_BITS, newy>>U_BITS));
            emit mousePressYUV(newx, newy);
            if(count > 2)
            {
                while(li->getCount())
                {
                    CvPoint p = li->nextPoint();
                    emit mousePressYUV(p.x<<V_BITS,p.y<<U_BITS);
                }

            }
        }
        else
            emit mouseMoveYUV();
    }
    x = newx;
    y = newy;
}

void YUVLabel::wheelEvent(QWheelEvent *ev)
{
    delta = ev->delta();
    ticks = delta/120;
    emit mouseScrollYUV(ticks);
}

void YUVLabel::leaveEvent(QEvent *)
{
    emit mouseLeaveYUV();
}

