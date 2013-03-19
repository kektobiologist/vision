#ifndef CAMERAWORKER_H
#define CAMERAWORKER_H

#include <QObject>
#include <QtCore>
#include <QThread>
#include <highgui/highgui.hpp>
#include <core/core.hpp>
#include <imgproc/imgproc.hpp>
#include <QImage>
#include <QPixmap>
#include <QMutex>
#include "yuv-conversions.h"
#include "lut3d.h"
#include "blob.h"
#include <vector>
#include "beliefstate.h"
#include "arena.h"
class CameraWorker : public QObject
{
    Q_OBJECT
public:
    explicit CameraWorker(QObject *parent = 0);
    void setup(QThread *cThread, QMutex *mutex, QMutex *_lutMutex, LUT3D *_lut);
    BeliefState *getBS() { return bs;}
signals:
    void imageReady(QPixmap *pm);
    void beliefStateReady(BeliefState *bs);
    void markBGR(int b, int g, int r);
public slots:
    void onMouseClicked(int x, int y);
    void onRightMouseDragged(int dx, int dy);
    void onMouseScrolled(int ticks);
    void onTimeout();
    void onEntry();
    void onStop();
    void onRadioToggle(bool cam);
    void onThresholdChanged(bool val);
    void onBlobChanged(bool val);
    void onArenaCheckBoxToggled(bool checked);
    void onMouseDraggedArenaCalib(int x,int y);
    void onMouseClickedArenaCalib(int x,int y);
private:
    bool isArenaCalib;
    Arena a;
    Blob *b;
    BeliefState *bs;
    std::vector<BlobData> blobDataArr;
    int delx, dely;
    int zoom;
    float *zoomArr;
    int lHeight, lWidth;
    IplROI *roi;
    bool isCamera;
    bool isThreshold;
    bool isBlob;
    IplImage *arenaFrame;
    IplImage *frame;
    IplImage *calibFrame;
    IplImage *displayFrame, *displayCamFrame, *displayArenaFrame;
    IplImage *blobImage;
    QMutex *myMutex, *lutMutex;
    QThread *myThread;
    QTimer *timer;
    CvCapture *capture;
    QPixmap *myPixmap;
    LUT3D *lut;
    CvPoint convertToCalibCoord(CvPoint in);
    void colorImage(IplImage *in, IplImage *out);
    void makeBlobImage(IplImage *src, IplImage *dst);
    void drawBlobs(IplImage *dst, std::vector<BlobData> blobDataArr);
};

#endif // CAMERAWORKER_H
