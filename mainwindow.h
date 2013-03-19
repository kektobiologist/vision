#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <highgui/highgui.hpp>
#include <core/core.hpp>
#include <imgproc/imgproc.hpp>
#include <QList>
#include <QListWidgetItem>
#include <QPixmap>
#include <QMutex>
#include <QTime>
//#include "cameraworker.h"
//#include "calibratorworker.h"
#include "beliefstate.h"
namespace Ui {
class MainWindow;
}
class CameraWorker;
class CalibratorWorker;
class AlgoWorker;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QTimer *timer;
public slots:
    void onCamImageReady(QPixmap *pm);
    void onYUVImageReady(QPixmap *pm);
    void onBeliefStateReady(BeliefState *bs);
    void onTimeout();
    void onMouseOnlyMove(int x, int y);
signals:
    void colorChanged(int row);
    void reset();
    void clearMarks();
    void stopCamThread();
    void stopCalibThread();
    void radioToggle(bool cam);
    void thesholdCheckChanged(bool val);
    void blobCheckChanged(bool val);
private slots:
    void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_camRadio_toggled(bool checked);

    void on_reset_clicked();

    void on_clearMarks_clicked();

    void on_thresh_checkbox_stateChanged(int arg1);

    void on_blobs_checkbox_stateChanged(int arg1);

    void on_startButton_clicked();

    void on_stopButton_clicked();


private:
    Ui::MainWindow *ui;
    QMutex *camMutex;
    QMutex *calibMutex;
    QMutex *lutMutex;
    QThread *cameraThread, *calibThread, *algoThread;
    CameraWorker *cw;
    CalibratorWorker *calibw;
    AlgoWorker *aw;
    QTime *fpsTimer;
};

#endif // MAINWINDOW_H
