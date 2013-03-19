#ifndef YUVCONVERSIONS_H
#define YUVCONVERSIONS_H
#include <stdio.h>
#include <highgui/highgui.hpp>
#include <core/core.hpp>
#include <opencv.hpp>
#ifndef IMGDATA
#define IMGDATA(image,i,j,k) (((uchar*)image->imageData)[(i)*(image->widthStep) + (j)*(image->nChannels) + (k)])
#endif
template <class num1,class num2>
 num1 bound(num1 x,num2 low,num2 high);

void rgb2yuv(int r, int g, int b, int & y, int & u, int & v);

void yuv2rgb(int y, int u, int v, int & r, int & g, int & b);

bool convert_yuv2bgr(IplImage *img);

bool convert_bgr2yuv(IplImage *img);


#endif // YUVCONVERSIONS_H
