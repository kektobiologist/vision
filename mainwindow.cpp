#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtCore>
#include <highgui/highgui.hpp>
#include <core/core.hpp>
#include <imgproc/imgproc.hpp>
#include <opencv.hpp>
#include "cameraworker.h"
#include "calibratorworker.h"
#include "algoworker.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    timer = new QTimer();
    for(int i=0; i<MAX_COLORS; i++)
    {
        ui->listWidget->addItem(LUT3D::getString((Color)i));
    }
    fpsTimer = new QTime;
    lutMutex = new QMutex;
//    connect(ui->imgLabel, SIGNAL(mouseOnlyMove(int,int)), this, SLOT(onMouseOnlyMove(int,int)));

    /////////////////////////////////// Setting up Calibrator worker:
    calibThread = new QThread;
    calibMutex = new QMutex;
    calibw = new CalibratorWorker;
    calibw->setup(calibThread, calibMutex, lutMutex, ui, this);
    connect(this, SIGNAL(stopCalibThread()), calibw, SLOT(onStop()));
    calibw->moveToThread(calibThread);
    calibThread->start(QThread::HighestPriority);

    /////////////////////////////////// Setting up Camera worker:
    cameraThread = new QThread;
    camMutex = new QMutex;
    cw = new CameraWorker;
    cw->setup(cameraThread, camMutex, lutMutex, calibw->getLUT());
    connect(this, SIGNAL(stopCamThread()), cw, SLOT(onStop()));
    connect(ui->imgLabel, SIGNAL(mousePressed(int,int)), cw, SLOT(onMouseClicked(int,int)));
    connect(ui->imgLabel, SIGNAL(mouseClicked(int,int)), cw, SLOT(onMouseClicked(int,int)));
    connect(ui->imgLabel, SIGNAL(mousePressed(int,int)), cw, SLOT(onMouseDraggedArenaCalib(int,int)));
    connect(ui->imgLabel, SIGNAL(mouseClicked(int,int)), cw, SLOT(onMouseClickedArenaCalib(int,int)));    
    connect(ui->imgLabel, SIGNAL(mouseRightPressed(int,int)), cw, SLOT(onRightMouseDragged(int,int)));
    connect(ui->imgLabel, SIGNAL(mouseScrollCam(int)), cw, SLOT(onMouseScrolled(int)));
    connect(ui->arena_checkBox, SIGNAL(toggled(bool)), cw, SLOT(onArenaCheckBoxToggled(bool)));
    connect(cw, SIGNAL(imageReady(QPixmap*)), this, SLOT(onCamImageReady(QPixmap*)));
    connect(cw, SIGNAL(beliefStateReady(BeliefState*)), this, SLOT(onBeliefStateReady(BeliefState*)));
    connect(this, SIGNAL(radioToggle(bool)), cw, SLOT(onRadioToggle(bool)));
    connect(this, SIGNAL(thesholdCheckChanged(bool)), cw, SLOT(onThresholdChanged(bool)));
    connect(this, SIGNAL(blobCheckChanged(bool)), cw, SLOT(onBlobChanged(bool)));
    cw->moveToThread(cameraThread);
    cameraThread->start();
//    timer->start(30);
    connect(cw, SIGNAL(markBGR(int,int,int)), calibw, SLOT(onCameraImageClicked_bgr(int,int,int)));

    ////////////////////////////////// Setting up Algorithm Worker:
    algoThread = new QThread;
    aw = new AlgoWorker;
    aw->setup(algoThread, camMutex, cw->getBS());
    aw->moveToThread(algoThread);
//    algoThread->start();

    fpsTimer->start();

}
void MainWindow::onTimeout()
{

}

void MainWindow::onMouseOnlyMove(int x, int y)
{
//    qDebug()<< "mousemove";
    ui->mouseXLabel->setText("x = " + QString::number(x));
    ui->mouseYLabel->setText("y = " + QString::number(y));
}

MainWindow::~MainWindow()
{
    emit stopCamThread();
    cameraThread->wait();
    emit stopCalibThread();
    calibThread->wait();
    delete calibw;
    delete cw;
    delete ui;
}

void MainWindow::onCamImageReady(QPixmap *pm)
{
    int nMilliseconds = fpsTimer->elapsed();
    ui->fpsLabel->setText("FPS = "+QString::number(1000.0/nMilliseconds));
    camMutex->lock();
    ui->imgLabel->setPixmap(*pm);
    camMutex->unlock();
    fpsTimer->restart();
}

void MainWindow::onYUVImageReady(QPixmap *pm)
{
    calibMutex->lock();
    ui->yuvLabel->setPixmap(*pm);
    calibMutex->unlock();
}

void MainWindow::onBeliefStateReady(BeliefState *bs)
{
    camMutex->lock();
    if(!bs)
    {
        camMutex->unlock();
        return;
    }
    if(bs->isBot)
    {
        ui->botPosLabel->setText("x, y = " + QString::number(bs->getBotPos().x) + ", " + QString::number((bs->getBotPos().y)));
        ui->botAngleLabel->setText("Angle = " + QString::number(bs->getBotAngle()));
    }
    else
    {
        ui->botPosLabel->setText("x, y = Not Found.");;
        ui->botAngleLabel->setText("Angle = Not Found.");
    }
    camMutex->unlock();
}

void MainWindow::on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    emit colorChanged((int)(ui->listWidget->row(current)));
}

void MainWindow::on_camRadio_toggled(bool checked)
{
    emit radioToggle(checked);
}

void MainWindow::on_reset_clicked()
{
    emit reset();
}

void MainWindow::on_clearMarks_clicked()
{
    emit clearMarks();
}

void MainWindow::on_thresh_checkbox_stateChanged(int arg1)
{
    emit thesholdCheckChanged((bool)arg1);
}

void MainWindow::on_blobs_checkbox_stateChanged(int arg1)
{
    emit blobCheckChanged((bool)arg1);
}

void MainWindow::on_startButton_clicked()
{
    if(!algoThread->isRunning())
        algoThread->start();
}

void MainWindow::on_stopButton_clicked()
{
//    qDebug() << "pressed stop";
    if(algoThread->isRunning())
    {
//        qDebug() << "ok";
        algoThread->quit();
//        algoThread->wait();
    }
}
