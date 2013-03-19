#ifndef LINEINTERPOLATE_H
#define LINEINTERPOLATE_H
#include <highgui/highgui.hpp>
#include <core/core.hpp>
#include <imgproc/imgproc.hpp>
#include <opencv.hpp>
#include <QtCore>
class LineInterpolate
{
private:
    CvLineIterator *iterator;
    IplImage *src;
    int count;
public:
    LineInterpolate(CvSize size);
    int init(CvPoint p1, CvPoint p2);
    CvPoint nextPoint();
    int getCount();
};

#endif // LINEINTERPOLATE_H
