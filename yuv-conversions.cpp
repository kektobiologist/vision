#include <stdio.h>
#include <highgui/highgui.hpp>
#include <core/core.hpp>
#include <opencv.hpp>
#ifndef IMGDATA
#define IMGDATA(image,i,j,k) (((uchar*)image->imageData)[(i)*(image->widthStep) + (j)*(image->nChannels) + (k)])
#endif
template <class num1,class num2>
num1 bound(num1 x,num2 low,num2 high)
{
  if(x < low ) x = low;
  if(x > high) x = high;
  return(x);
}
void rgb2yuv(int r, int g, int b, int & y, int & u, int & v)
{
  y = bound((306*r + 601*g + 117*b)  >> 10, 0, 255);
  u = bound(((-172*r - 340*g + 512*b) >> 10)  + 128, 0, 255);
  v = bound(((512*r - 429*g - 83*b) >> 10) + 128, 0, 255);
}
void yuv2rgb(int y, int u, int v, int & r, int & g, int & b)
{
  u = u-128;
  v = v-128;
  r = bound(y         + ((v*1436) >>10), 0, 255);
  g = bound(y - ((u*352 + v*731) >> 10), 0, 255);
  b = bound(y + ((u*1814) >> 10)       , 0, 255);
}
//void rgb2yuv(int r, int g, int b, int & y, int & u, int & v)
//{
////    y = r *  .299000 + g *  .587000 + b *  .114000;
////    u = r * -.168736 + g * -.331264 + b *  .500000 + 128.0;
////    v = r *  .500000 + g * -.418688 + b * -.081312 + 128.0;
////    y = bound(y, 0, 255);
////    u = bound(u, 0, 255);
////    v = bound(v, 0, 255);
//    y=r;
//    u=g;
//    v=b;
////    y = (r+2*g+b)/4.0;
////        u = b-g;
////        v = r-g;
//}
//void yuv2rgb(int y, int u, int v, int & r, int & g, int & b)
//{
////    r = y + 1.4075 * (v - 128.0);
////    g = y - 0.3455 * (u - 128.0) - (0.7169 * (v - 128.0));
////    b = y + 1.7790 * (u - 128.0);
////    r = bound(r, 0, 255);
////    g = bound(g, 0, 255);
////    b = bound(b, 0, 255);
//    r=y;
//    g=u;
//    b=v;
////    g = y - (u+v)/4.0+1;
////        r = v+g;
////        b = u+g;
//}
bool convert_yuv2bgr(IplImage *img)
{
    if(!img)
    {
        return 0;
    }
    int h = img->height;
    int w = img->width;
    for (int i = 0; i < h; ++i)
    {
        for (int j = 0; j < w; ++j)
        {
            int r, g, b;
            yuv2rgb(IMGDATA(img, i, j, 0), IMGDATA(img, i, j, 1), IMGDATA(img, i, j, 2), r, g, b);
            IMGDATA(img, i, j, 2) = r;
            IMGDATA(img, i, j, 1) = g;
            IMGDATA(img, i, j, 0) = b;
        }
    }
    return 1;
}
bool convert_bgr2yuv(IplImage *img)
{
    if(!img)
    {
        return 0;
    }
    int h = img->height;
    int w = img->width;
    for (int i = 0; i < h; ++i)
    {
        for (int j = 0; j < w; ++j)
        {
            int y, u, v;
            rgb2yuv(IMGDATA(img, i, j, 2), IMGDATA(img, i, j, 1), IMGDATA(img, i, j, 0), y, u, v);
            IMGDATA(img, i, j, 0) = (y);
            IMGDATA(img, i, j, 1) = (u);
            IMGDATA(img, i, j, 2) = (v);
        }
    }
    return 1;
}
