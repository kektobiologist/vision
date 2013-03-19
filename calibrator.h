#ifndef CALIBRATOR_H
#define CALIBRATOR_H
#include "lut3d.h"
//#include "yuv-conversions.h"
#include <stdio.h>
#include <pthread.h>
#include <QThread>
#include <QtCore>
#define Y_BITS 4 //cell size in bits
#define U_BITS 3
#define V_BITS 2
#define INBOUNDS(x, min, max) ((x>= (min)) && (x<=(max)))
#define CONFIG_FILE "lut-config"
#define LOGIC_YUV(yL,uL,vL) (arrLogic[(yL)*U_LEN*V_LEN + (uL)*V_LEN + (vL)])
#define MARK_YUV(yL,uL,vL) (arrMark[(yL)*U_LEN*V_LEN + (uL)*V_LEN + (vL)])
#define ISDIFF_COORD(x,y,prevx,prevy) (((x)>>V_BITS)!= ((prevx)>>V_BITS) || ((y)>>U_BITS)!= ((prevy)>>U_BITS))
static const int CELL_Y = (1 << Y_BITS);
static const int CELL_U = (1 << U_BITS);
static const int CELL_V = (1 << V_BITS);

static const int Y_LEN = 256 / CELL_Y;
static const int U_LEN = 256 / CELL_U;
static const int V_LEN = 256 / CELL_V;
static const int NUM_SLICES = Y_LEN;
static const int LOGIC_SIZE = Y_LEN * U_LEN * V_LEN;
struct Triplet{
    int y,u,v;
    Triplet(int y, int u, int v): y(y), u(u), v(v) {}
    Triplet(): y(0), u(0), v(0) {}
};

class Calibrator
{
private:
    IplImage *imgSlices[NUM_SLICES]; //Stores in BGR. Base data to be copied from when logic is UNDEF.
    IplImage *imgDisplay[NUM_SLICES]; //Stores in BGR (display)
    char *arrLogic; //Stores user-driven color logic in YUV (shrunk), UNDEF if not defined.
    char *arrMark; //Stores if a cell is marked. 0 = unmarked, 1 = marked. size same as logic.
    Color curColor;
    QMutex *lutMutex;
    char *configName;
    int idx;
    bool toPaint;
    pthread_mutex_t mutex;
    void lock();
    void unlock();
    void saveLogic();
    void loadLogic();
    void clearLogic();
    void addColorDisplay(int yL, int uL, int vL);
    void addColorLUT(int yL, int uL, int vL);
    void addColorLUTColorOnly(int yL, int uL, int vL);
    void addMarkDisplay(int yL, int uL, int vL);
    void _floodFill(std::queue<Triplet> &q, int yL, int uL, int vL, Color color);
    void floodFill(int yL, int uL, int vL);
    void initDisplayFromLogic();
    void initLUTFromLogic();
public:
    Calibrator(QMutex *mutex);
    ~Calibrator();
    IplImage *getImage();
    void setPaint(){toPaint = true;}
    void clearPaint(){toPaint = false;}
    void onColorChange(Color newColor);
    void onMouseScroll(int ticks);
    void onMouseClick(int x, int y);
    void onMouseRightClick(int x, int y);
    void mark_bgr(int b, int g, int r); //marks imgDisplay with X's at bgr->yuv value
    void colorImage(IplImage *in, IplImage *out); //colors output with thresholded colors. Keeps source color if color is UNDEF.
    void clearMarks(); //also removes marks from display
    void reset(); //clears everything
    LUT3D *lut;
};
#endif
