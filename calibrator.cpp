#include "calibrator.h"
#include <queue>

void Calibrator::lock()
{
    lutMutex->lock();
}
void Calibrator::unlock()
{
    lutMutex->unlock();
}
Calibrator::Calibrator(QMutex *mutex)
{
    lutMutex = mutex;
    lut = new LUT3D;
    configName = new char[100];
    sprintf(configName, "%s_%dx%dx%d", CONFIG_FILE, Y_LEN, U_LEN, V_LEN);
    curColor = UNDEF;
    idx=0;
    setPaint();
    arrLogic = new char[LOGIC_SIZE+2];
    if(!arrLogic)
    {
        printf("Error! arrLogic is NULL\n");
        exit(0);
    }
    loadLogic();
    arrMark = new char[LOGIC_SIZE+2];
    if(!arrMark)
    {
        printf("Error! arrMark is NULL\n");
        exit(0);
    }
    for (int i = 0; i < LOGIC_SIZE; ++i)
    {
        arrMark[i] = 0;
    }
    for (int i = 0; i < Y_LEN; ++i)
    {
        imgSlices[i] = cvCreateImage(cvSize(256, 256), 8, 3);
        for (int it = 0; it < 256; ++it)
        {
            for (int jt = 0; jt < 256; ++jt)
            {
                IMGDATA(imgSlices[i], it, jt, 0) = (i * 256) / Y_LEN ;
                IMGDATA(imgSlices[i], it, jt, 1) = it;
                IMGDATA(imgSlices[i], it, jt, 2) = jt;
            }
        }
        cvCvtColor(imgSlices[i], imgSlices[i], CV_YUV2BGR);
        imgDisplay[i] = cvCloneImage(imgSlices[i]);
    }
    initDisplayFromLogic();
    initLUTFromLogic();

}
void Calibrator::reset()
{
    clearLogic();
//    clearMarks();
//    initLUTFromLogic();
    for(int b=0; b<256; b++)
        for(int g=0; g<256; g++)
            for(int r=0; r<256; r++)
                lut->set_bgr(b,g,r, UNDEF);
    initDisplayFromLogic();

}
void Calibrator::saveLogic()
{
    arrLogic[LOGIC_SIZE] = 0;
    for (int i = 0; i < LOGIC_SIZE; ++i)
    {
        arrLogic[i] += 'a';
    }
    FILE *f = fopen(configName, "w");
    if(!f)
    {
        printf("Error! file could not be saved.\n");
        return;
    }
    int c=0;
    if(!(c=fprintf(f, "%s\n", arrLogic)))
        printf("data couldnt be saved?\n");
    else
        qDebug() << "data saved = " << c ;
    fclose(f);
}
void Calibrator::loadLogic()
{
    clearLogic();
    FILE *f = fopen(configName, "r");
    if(!f)
    {
        printf("Config file not found.");
        return;
    }
    fscanf(f, "%s\n", arrLogic);
    if(strlen(arrLogic) != LOGIC_SIZE)
    {
        printf("Error! data loaded size is %d, required %d.\n", strlen(arrLogic), LOGIC_SIZE);
        fclose(f);
        return;
    }
    for (int i = 0; i < LOGIC_SIZE; ++i)
    {
        arrLogic[i] -= 'a';
    }
    fclose(f);
}
void Calibrator::clearLogic()
{
    for (int i = 0; i < LOGIC_SIZE; ++i)
    {
        arrLogic[i] = UNDEF;
    }
}
IplImage *Calibrator::getImage()
{
    if (idx >= NUM_SLICES || idx < 0)
    {
        printf("Error: img idx out of range\n");

        return NULL;
    }
    IplImage *ret = imgDisplay[idx];
    return ret;
}
void Calibrator::onColorChange(Color newColor)
{
    if(newColor < 0 || newColor >= MAX_COLORS)
    {
        printf("error! new color out of bounds.\n");
        return;
    }
    curColor = newColor;
    setPaint();
}
void Calibrator::onMouseScroll(int ticks)
{
    idx += ticks;
    idx = idx>=NUM_SLICES?NUM_SLICES-1:idx<0?0:idx;
    setPaint();
}
void Calibrator::onMouseClick(int x, int y)
{
    if(x<0||x>255||y<0||y>255)
    {
        return;
    }
    int yL = idx;
    int uL = (y >> U_BITS);
    int vL = (x >> V_BITS);
    LOGIC_YUV(yL, uL, vL) = curColor;
    addColorDisplay(yL, uL, vL);
    addColorLUT(yL, uL, vL);
    addColorLUTColorOnly(yL, uL, vL);
    setPaint();
}

void Calibrator::onMouseRightClick(int x, int y)
{
    if(x<0||x>255||y<0||y>255)
    {
        return;
    }
    int yL = idx;
    int uL = (y >> U_BITS);
    int vL = (x >> V_BITS);
    if(LOGIC_YUV(yL, uL, vL) == curColor)
    {
        return;
    }
    floodFill(yL, uL, vL);

}
void Calibrator::addColorDisplay(int yL, int uL, int vL) // parameters are in logic space.
{
    int whichIdx = yL;
    int ymin = yL<<Y_BITS;
    int umin = uL<<U_BITS;
    int vmin = vL<<V_BITS;
    int ymax = ymin + CELL_Y -1;
    int umax = umin + CELL_U -1;
    int vmax = vmin + CELL_V -1;
    Color c = (Color)LOGIC_YUV(yL, uL, vL);
    lock();
    CvScalar cs = lut->getScalar(c);
    unlock();
    int bc = cs.val[0];
    int gc = cs.val[1];
    int rc = cs.val[2];
    for (int u = umin; u <= umax; ++u)
    {
        for (int v = vmin; v <= vmax; ++v)
        {
            if(c != UNDEF)
            {
                IMGDATA(imgDisplay[whichIdx], u, v, 0) = bc;
                IMGDATA(imgDisplay[whichIdx], u, v, 1) = gc;
                IMGDATA(imgDisplay[whichIdx], u, v, 2) = rc;
            }
            else
            {
                IMGDATA(imgDisplay[whichIdx], u, v, 0) = IMGDATA(imgSlices[whichIdx], u, v, 0);
                IMGDATA(imgDisplay[whichIdx], u, v, 1) = IMGDATA(imgSlices[whichIdx], u, v, 1);
                IMGDATA(imgDisplay[whichIdx], u, v, 2) = IMGDATA(imgSlices[whichIdx], u, v, 2);
            }
        }
    }
    addMarkDisplay(yL, uL, vL);
}
void Calibrator::addColorLUT(int yL, int uL, int vL)
{
    // add code for editing LUT
    int ymin = yL<<Y_BITS;
    int umin = uL<<U_BITS;
    int vmin = vL<<V_BITS;
    int ymax = ymin + CELL_Y -1;
    int umax = umin + CELL_U -1;
    int vmax = vmin + CELL_V -1;
    Color c = (Color)LOGIC_YUV(yL, uL, vL);
    lock();
    for (int y = ymin; y < ymax+1; ++y)
    {
        for (int u = umin; u < umax+1; ++u)
        {
            for (int v = vmin; v < vmax+1; ++v)
            {
                int r, g, b;
//                int yp=y,up=u,vp=v;
                yuv2rgb(y, u, v, r, g, b);
//                rgb2yuv(r, g, b, y, u, v);
//                if(yp!=y || up!=u || vp!=v)
//                    qDebug() << "yuv2rgb and rgb2yuv not interconvbertible!";
//                y=yp;u=up;v=vp;

                for(int it=-1;it<=1;it++)
                    for(int jt=-1;jt<=1;jt++)
                        for(int kt=-1;kt<=1;kt++)                            
                            lut->set_bgr(b+it, g+jt, r+kt, c);

//                lut->set_bgr(b,g,r,c);
            }
        }
    }        
    unlock();
}

void Calibrator::addColorLUTColorOnly(int yL, int uL, int vL)
{
    // add code for editing LUT
    int ymin = yL<<Y_BITS;
    int umin = uL<<U_BITS;
    int vmin = vL<<V_BITS;
    int ymax = ymin + CELL_Y -1;
    int umax = umin + CELL_U -1;
    int vmax = vmin + CELL_V -1;
    Color c = (Color)LOGIC_YUV(yL, uL, vL);
    lock();
    if(c != UNDEF)
    for (int y = ymin; y < ymax+1; ++y)
    {
        for (int u = umin; u < umax+1; ++u)
        {
            for (int v = vmin; v < vmax+1; ++v)
            {
                int r, g, b;
//                int yp=y,up=u,vp=v;
                yuv2rgb(y, u, v, r, g, b);
//                rgb2yuv(r, g, b, y, u, v);
//                if(yp!=y || up!=u || vp!=v)
//                    qDebug() << "yuv2rgb and rgb2yuv not interconvbertible!";
//                y=yp;u=up;v=vp;

                for(int it=-1;it<=1;it++)
                    for(int jt=-1;jt<=1;jt++)
                        for(int kt=-1;kt<=1;kt++)
                            lut->set_bgr(b+it, g+jt, r+kt, c);

//                lut->set_bgr(b,g,r,c);
            }
        }
    }
    unlock();
}
void Calibrator::initDisplayFromLogic()
{
    for (int yL = 0; yL < Y_LEN; ++yL)
    {
        for (int uL = 0; uL < U_LEN; ++uL)
        {
            for (int vL = 0; vL < V_LEN; ++vL)
            {
                addColorDisplay(yL, uL, vL);
            }
        }
    }
}
void Calibrator::initLUTFromLogic()
{
    for (int yL = 0; yL < Y_LEN; ++yL)
    {
        for (int uL = 0; uL < U_LEN; ++uL)
        {
            for (int vL = 0; vL < V_LEN; ++vL)
            {
                addColorLUT(yL, uL, vL);
            }
        }
    }
    for (int yL = 0; yL < Y_LEN; ++yL)
    {
        for (int uL = 0; uL < U_LEN; ++uL)
        {
            for (int vL = 0; vL < V_LEN; ++vL)
            {
                addColorLUTColorOnly(yL, uL, vL);
            }
        }
    }
}
void Calibrator::mark_bgr(int b, int g, int r)
{
    int y, u, v;
    rgb2yuv(r, g, b, y, u, v);
    int yL = y>>Y_BITS;
    int uL = u>>U_BITS;
    int vL = v>>V_BITS;
    MARK_YUV(yL,uL,vL) = 1;
    addColorDisplay(yL, uL, vL);
}
void Calibrator::addMarkDisplay(int yL, int uL, int vL)
{
    CvScalar markColor;
    int b = IMGDATA(imgDisplay[yL],(uL<<U_BITS), (vL<<V_BITS), 0);
    int g = IMGDATA(imgDisplay[yL],(uL<<U_BITS), (vL<<V_BITS), 1);
    int r = IMGDATA(imgDisplay[yL],(uL<<U_BITS), (vL<<V_BITS), 2);
    int gray= (0.299*r + 0.587*g + 0.114*b);
    if(gray > 128)
        markColor = cvScalar(0,0,0);
    else
        markColor = cvScalar(255, 255, 255);
    if(MARK_YUV(yL, uL, vL))
    {
        int umin = uL<<U_BITS;
        int vmin = vL<<V_BITS;
        int umax = umin + CELL_U -1;
        int vmax = vmin + CELL_V -1;
        cvLine(imgDisplay[yL], cvPoint(vmin, umin), cvPoint(vmax, umax), markColor);
        cvLine(imgDisplay[yL], cvPoint(vmin, umax), cvPoint(vmax, umin), markColor);
    }
}

void Calibrator::_floodFill(std::queue<Triplet> &q, int yL, int uL, int vL, Color color)
{
    if(uL<0||uL>=U_LEN||vL<0||vL>=V_LEN)
        return;
    if(LOGIC_YUV(yL,uL,vL) == color)
        return;
    LOGIC_YUV(yL,uL,vL) = color;
    addColorDisplay(yL, uL, vL);
    addColorLUT(yL, uL, vL);
    addColorLUTColorOnly(yL, uL, vL);
    q.push(Triplet(yL, uL, vL));
}

void Calibrator::floodFill(int yL, int uL, int vL)
{
    Color color = curColor;
    std::queue<Triplet> q;
    LOGIC_YUV(yL,uL,vL) = color;
    addColorDisplay(yL, uL, vL);
    addColorLUT(yL, uL, vL);
    addColorLUTColorOnly(yL, uL, vL);
    q.push(Triplet(yL, uL, vL));
    Triplet curT;
    while(q.size())
    {
        curT = q.front();
        q.pop();
        int yC = curT.y;
        int uC = curT.u;
        int vC = curT.v;
        _floodFill(q, yC, uC+1, vC, color);
        _floodFill(q, yC, uC-1, vC, color);
        _floodFill(q, yC, uC, vC+1, color);
        _floodFill(q, yC, uC, vC-1, color);
    }

}
void Calibrator::clearMarks()
{
    for (int yL = 0; yL < Y_LEN; ++yL)
    {
        for (int uL = 0; uL < U_LEN; ++uL)
        {
            for (int vL = 0; vL < V_LEN; ++vL)
            {
                if(MARK_YUV(yL, uL, vL))
                {
                    MARK_YUV(yL, uL, vL) = 0;
                    addColorDisplay(yL, uL, vL);
                }
            }
        }
    }

}
void Calibrator::colorImage(IplImage *in, IplImage *out) //WARNING: DON'T CALL
{

    if(!in)
    {
        printf("input is empty.\n");
        return;
    }
    if(in->height != out->height || in->width != out->width || in->nChannels != out->nChannels)
    {
        printf("input - output sizes dont match!\n");
        return;
    }
    lock();
    for (int i = 0; i < in->height; ++i)
    {
        for (int j = 0; j < in->width; ++j)
        {
            int b = IMGDATA(in, i, j, 0);
            int g = IMGDATA(in, i, j, 1);
            int r = IMGDATA(in, i, j, 2);
            CvScalar cs = lut->getScalar(lut->lookup(b, g, r));
            if(lut->lookup(b, g, r) == UNDEF)
            {
                IMGDATA(out, i, j, 0) = b;
                IMGDATA(out, i, j, 1) = g;
                IMGDATA(out, i, j, 2) = r;
            }
            else
            {
                IMGDATA(out, i, j, 0) = cs.val[0];
                IMGDATA(out, i, j, 1) = cs.val[1];
                IMGDATA(out, i, j, 2) = cs.val[2];
            }
        }
    }
    unlock();
}
Calibrator::~Calibrator()
{
//    qDebug() << "calib being destroyed.";
    saveLogic();
    for (int i = 0; i < NUM_SLICES; ++i)
    {
        if(imgSlices[i])
            cvReleaseImage(&imgSlices[i]);
        if(imgDisplay[i])
            cvReleaseImage(&imgDisplay[i]);
    }
    if(configName)
        delete[] configName;
    if(arrLogic)
        delete[] arrLogic;
    if(lut)
        delete lut;
//    qDebug() << "calib destroyed.";
}
