#include "lineinterpolate.h"

LineInterpolate::LineInterpolate(CvSize size)
{
    src = cvCreateImage(size, 8, 3);
    iterator = new CvLineIterator;
    count = 0;
}

int LineInterpolate::init(CvPoint p1, CvPoint p2)
{
    count = cvInitLineIterator( src, p1, p2, iterator, 8, 0 );
//    qDebug() << "p1 = " << p1.x << ", " << p1.y << ". p2 = "<< p2.x << ", " << p2.y;
    return count;
}

CvPoint LineInterpolate::nextPoint()
{
    if(!count)
    {
        qDebug() <<"too many points requested.";
        return cvPoint(-1, -1);
    }
    int offset=0, x=0, y=0;
    CV_NEXT_LINE_POINT(*iterator);
    offset = iterator->ptr - (uchar*)(src->imageData);
    y = offset/src->widthStep;
    x = (offset - y*src->widthStep)/(3*sizeof(uchar));
    count--;
    return cvPoint(x,y);
}

int LineInterpolate::getCount()
{
    return count;
}
