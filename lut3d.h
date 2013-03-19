#ifndef LUT3D_H
#define LUT3D_H

#include <stdio.h>
#include <highgui/highgui.hpp>
#include <queue>
#include <vector>
#include "yuv-conversions.h"
using namespace std;
#ifndef IMGDATA
#define IMGDATA(image,i,j,k) (((uchar*)image->imageData)[(i)*(image->widthStep) + (j)*(image->nChannels) + (k)])
#endif
#ifndef LOOKUP_BGR
#define LOOKUP_BGR(b,g,r) (lut[(b<<16)+(g<<8)+r])
#endif
enum Color{
    UNDEF,
    WHITE,
    BLACK,
    ORANGE,
    BLUE,
    YELLOW,
    PINK,
    GREEN,
    CYAN,
    FIELD_GREEN,
    RED,
    MAX_COLORS
};

class LUT3D
{
public:
    LUT3D();
    void reset();
    void set_bgr(int b, int g, int r, Color c);
    void set_yuv(int y, int u, int v, Color c);
    Color get_bgr(int b, int g, int r);
    CvScalar getScalar(Color c);
    static const char *getString(Color c);
    Color lookup(int b, int g, int r) {return (Color)LOOKUP_BGR(b,g,r);}
    ~LUT3D();
private:
    char *lut;
    void saveLUT();
    CvScalar colorValues[MAX_COLORS];
    static const char *colorString[MAX_COLORS];
    int size;
};


#endif // LUT3D_H
