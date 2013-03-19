#ifndef CALIBRATORWORKER_H
#define CALIBRATORWORKER_H

#include <QObject>
#include <QtCore>
#include <QThread>
#include <highgui/highgui.hpp>
#include <core/core.hpp>
#include <imgproc/imgproc.hpp>
#include <QImage>
#include <QPixmap>
#include <QMutex>
#include "calibrator.h"
#include "yuvlabel.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

class CalibratorWorker : public QObject
{
    Q_OBJECT
public:
    explicit CalibratorWorker(QObject *parent = 0);
    void setup(QThread *cThread, QMutex *mutex, QMutex *_lutMutex, Ui::MainWindow *ui, MainWindow *mw);
    LUT3D *getLUT();
signals:
    void myPaint();
    void imageReady(QPixmap *pm);
public slots:
    void onEntry();
    void onMousePress(int x, int y);
    void onMouseRightPress(int x, int y);
    void onMouseScroll(int ticks);
    void onColorChanged(int row);
    void onCameraImageClicked_bgr(int b, int g, int r);
    void onReset();
    void onClearMarks();
    void onMyPaint();
    void onStop();
private slots:

private:
    Ui::MainWindow *uiPtr;
    MainWindow *mw;
    IplImage *frame;
    QPixmap *myPixmap;
    QThread *myThread;
    QMutex *myMutex, *lutMutex;
    Calibrator *cb;
};

#endif // CALIBRATORWORKER_H
