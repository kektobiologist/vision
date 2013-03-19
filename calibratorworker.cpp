#include "calibratorworker.h"

CalibratorWorker::CalibratorWorker(QObject *parent) :
    QObject(parent)
{
    myPixmap = NULL;
}

void CalibratorWorker::setup(QThread *cThread, QMutex *mutex, QMutex *_lutMutex, Ui::MainWindow *ui, MainWindow *_mw)
{
    lutMutex = _lutMutex;
    myThread = cThread;
    myMutex = mutex;
    uiPtr = ui;
    mw = _mw;
    cb = new Calibrator(lutMutex);
    connect(cThread, SIGNAL(started()), this, SLOT(onEntry()));
}

LUT3D *CalibratorWorker::getLUT()
{
    return cb->lut;
}

void CalibratorWorker::onMousePress(int x, int y)
{
    cb->onMouseClick(x, y);
    emit myPaint();
}

void CalibratorWorker::onMouseRightPress(int x, int y)
{
    cb->onMouseRightClick(x, y);
    emit myPaint();
}

void CalibratorWorker::onMouseScroll(int ticks)
{
    cb->onMouseScroll(ticks);
    emit myPaint();
}

void CalibratorWorker::onColorChanged(int row)
{
    cb->onColorChange((Color)row);
    emit myPaint();
}

void CalibratorWorker::onCameraImageClicked_bgr(int b, int g, int r)
{
//    qDebug() << "click! calib";
    cb->mark_bgr(b, g, r);
    emit myPaint();
}

void CalibratorWorker::onReset()
{
    cb->reset();
    emit myPaint();
}

void CalibratorWorker::onClearMarks()
{
    cb->clearMarks();
    emit myPaint();
}

void CalibratorWorker::onMyPaint()
{
    frame = cb->getImage();
    cvCvtColor(frame, frame, CV_BGR2RGB);
    QImage qimg((uchar*)frame->imageData, frame->width, frame->height, frame->widthStep, QImage::Format_RGB888);
    myMutex->lock();
    if(myPixmap)
        delete myPixmap;
    myPixmap = new QPixmap(QPixmap::fromImage(qimg));
    myMutex->unlock();
    emit imageReady(myPixmap);
    cvCvtColor(frame, frame, CV_RGB2BGR);
}

void CalibratorWorker::onStop()
{
    qDebug() << "Stopped calib thread.";
    if(cb)
        delete cb;
    myThread->exit();
}

void CalibratorWorker::onEntry()
{
    qDebug() << "Entered Calibrator worker.";
    connect(this, SIGNAL(myPaint()), this, SLOT(onMyPaint()));
    connect(uiPtr->yuvLabel, SIGNAL(mousePressYUV(int,int)), this, SLOT(onMousePress(int,int)));
    connect(uiPtr->yuvLabel, SIGNAL(mouseScrollYUV(int)), this, SLOT(onMouseScroll(int)));
    connect(uiPtr->yuvLabel, SIGNAL(mouseRightPressYUV(int,int)), this, SLOT(onMouseRightPress(int,int)));
    connect(mw, SIGNAL(colorChanged(int)), this, SLOT(onColorChanged(int)));
    connect(mw, SIGNAL(reset()), this, SLOT(onReset()));
    connect(mw, SIGNAL(clearMarks()), this, SLOT(onClearMarks()));
    connect(this, SIGNAL(imageReady(QPixmap*)), mw, SLOT(onYUVImageReady(QPixmap*)));
    emit myPaint();
}

