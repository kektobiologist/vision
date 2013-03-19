#include "lut3d.h"
const char* LUT3D::colorString[] = {"Clear",
                                    "White",
                                    "Black",
                                    "Orange",
                                    "Blue",
                                    "Yellow",
                                    "Pink",
                                    "Green",
                                    "Cyan",
                                    "Field Green",
                                    "Red" };
LUT3D::LUT3D()
{
    size = 0x01 << (8+8+8);
    lut = new char[size];
    if(!lut)
    {
        printf("Error! LUT not allocated.\n");
        exit(0);
    }
    for (int i = 0; i < size; ++i)
        lut[i] = UNDEF;
    colorValues[UNDEF]  = CV_RGB(-1, -1, -1);
    colorValues[WHITE]  = CV_RGB(255, 255, 255);
    colorValues[BLACK]  = CV_RGB(0, 0, 0);
    colorValues[ORANGE] = CV_RGB(255, 128, 0);
    colorValues[BLUE]   = CV_RGB(0, 0, 255);
    colorValues[YELLOW] = CV_RGB(255, 255, 0);
    colorValues[PINK]   = CV_RGB(255, 0, 255);
    colorValues[GREEN]  = CV_RGB(0, 255, 0);
    colorValues[CYAN]   = CV_RGB(0, 255, 255);
    colorValues[FIELD_GREEN] = CV_RGB(0, 128, 0);
    colorValues[RED]    = CV_RGB(255, 0, 0);

}
const char *LUT3D::getString(Color c)
{
    if(c >= MAX_COLORS)
    {
        printf("Error: color out of bounds!\n");
        return NULL;
    }
    return colorString[c];
}
void LUT3D::reset()
{
    for (int i = 0; i < size; ++i)
    {
        lut[i]= UNDEF;
    }
}
void LUT3D::set_bgr(int b, int g, int r, Color c)
{
#define WR(a,m) ((a>=0) && (a<=m))
    if(c >= MAX_COLORS)
    {
        printf("Error: color out of bounds!\n");
        return;
    }
    if(WR(b,255) && WR(g,255) && WR(r,255))
        LOOKUP_BGR(b,g,r) = c;
#undef WR
}
void LUT3D::set_yuv(int y, int u, int v, Color c)
{
    if(c >= MAX_COLORS)
    {
        printf("Error: color out of bounds yuv!\n");
        return;
    }
    int b, g, r;
    yuv2rgb(y, u, v, r, g, b);
    LOOKUP_BGR(b,g,r) = c;
}
Color LUT3D::get_bgr(int b, int g, int r)
{
    return (Color)LOOKUP_BGR(b,g,r);
}
CvScalar LUT3D::getScalar(Color c)
{
    if(c >= MAX_COLORS)
    {
        printf("Error: color out of bounds scalar!\n");
        return colorValues[UNDEF];
    }
    return colorValues[c];
}
LUT3D::~LUT3D()
{
    saveLUT();
    if(lut)
        delete[] lut;
}

void LUT3D::saveLUT()
{
    if(!lut)
        return;
    FILE *f = fopen("lut-bgr", "w");
    for(int i=0; i<size; i++)
        lut[i] += 'a';
    if(!f)
        return;
    fprintf(f, "%s\n", lut);
    for(int i=0; i<size; i++)
        lut[i] -= 'a';
    fclose(f);
}
