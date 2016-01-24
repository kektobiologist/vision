#include "algoworker.h"
#include <QCoreApplication>
#include "config.h"
#include "serial.h"
#include <stdio.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#define WP_FILE "/home/arpit/sheldon/3/wp.txt"

AlgoWorker::AlgoWorker(QObject *parent) :
    QObject(parent)
{
    resourceCubeDest = cvPoint(200, 200); //Currently kept constant.
    numWood = numSilver = numGold = numBombs = 0;
    curCube = CubeData();
    curDest = cvPoint(-1, -1);
    isInterCube = false;
    isInterCubeCaptured = false;
}

void AlgoWorker::setup(QThread *cThread, QMutex *_bsMutex, BeliefState *_bs)
{
    if(!cThread)
    {
        qDebug() << "Thread is NULL!";
        return;
    }
    myThread = cThread;
    if(!_bsMutex)
    {
        qDebug() << "bs mutex NULL";
        return;
    }
    bsMutex = _bsMutex;
    bsMutex->lock();
    if(!_bs)
    {
        qDebug() << "belief state NULL!";
        bsMutex->unlock();
        return;
    }
    bsCam = _bs;
//    resourceCubeDest = bsCam->resourceDepositPoint;
    bsMutex->unlock();
    connect(myThread, SIGNAL(started()), this, SLOT(onEntry()));
    connect(myThread, SIGNAL(finished()), this, SLOT(onEnd()));
    timer = new QTimer;
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

void AlgoWorker::onEntry()
{
    cv::Vec3b front, back;
    FILE *f = fopen(WP_FILE, "r");
    if (!f) {
        qDebug() << "Could not open wp file.";
        return;
    }
  fscanf(f, "%d %d %d\n", &front[0], &front[1], &front[2]);
  fscanf(f, "%d %d %d\n", &back[0], &back[1], &back[2]);
  cout << "front: " << front << "back " << back << endl;
  int n = 0;
  fscanf(f, "%d\n", &n);
  x = vector<int>(n, 0);
  y = x;
  for (int i = 0; i < n; ++i)
  {
    fscanf(f, "%d %d\n", &x[i], &y[i]);
  }
    idx = 0;
    qDebug() << "Algo Worker started.";
    if(!s.Open("/dev/rfcomm0", 9600))
    {
        qDebug() << "Couldn't Open COM Port.";
        return;
    }
    else
    {
        qDebug() << "COM Port opened! =)";
    }
    s.WriteByte('O');
    s.WriteByte('l');
    botState = BOT_START;
    curDest = cvPoint(-1, -1);
    curCube = CubeData();
    numWood = numSilver = numGold = numBombs = 0;
    isInterCube = false;
    isInterCubeCaptured = false;
    timer->setSingleShot(true);
    timer->start(30);
}

void AlgoWorker::onEnd()
{
    s.WriteByte('O');
    s.WriteByte('l');
    botState = BOT_START;
    s.Close();
    qDebug() << "Algo Worker stopped.";
}

void AlgoWorker::onTimeout()
{
    bsMutex->lock();
    bs = *bsCam;
    bsMutex->unlock();
    if (idx >= x.size())
        return;
//    qDebug() << bs.botPos.x << "/// " << bs.botPos.y;
    //Do work.
//    qDebug() << "Inter = " << bs.startCorner.x << ",  " << bs.startCorner.y;
//    qDebug() << "mine deposit = " << bs.resourceDepositPoint.x << ", " << bs.resourceDepositPoint.y;
//    qDebug() << "bomb deposit = "  << bs.bombDepositPoint.x << ", " << bs.bombDepositPoint.y;

//    emit currentStateReady(botState);
//    qDebug() << "x, y = " << bs.getBotPos().x << ", " << bs.getBotPos().y;
    if (!moveToPoint(bs.getBotPos(), bs.getBotAngle(), cvPoint(x[idx], y[idx]))) {
      qDebug() << "Reached checkpoint" << idx;
      idx++;
      qDebug() << "NExt point: " << x[idx] << y[idx];
    }
    timer->setSingleShot(true);
    timer->start(30);
}

void AlgoWorker::execCommand(BotCommand com)
{
    char c ='x';
    bool toSleep = false;
    int pulseMs = 0;
    int sleepMs = 0;
    switch(com)
    {
    case BOT_RIGHT:
        c = 'd';
        break;
    case BOT_LEFT:
        c = 'a';
        break;
    case BOT_FORWARD:
        c = 'W';
        break;
    case BOT_BACKWARD:
        c = 'S';
        break;
    case BOT_STOP:
        c = 'O';
        break;
    case BOT_LED_ON:
        c = 'L';
        break;
    case BOT_LED_OFF:
        c = 'l';
        break;
    case BOT_F_PULSE:
        c = 'W';
        toSleep = true;
        pulseMs = 100;
        sleepMs = 200;
        break;
    case BOT_B_PULSE:
        c = 's';
        toSleep = true;
        pulseMs = 200;
        sleepMs = 400;
        break;
    case BOT_R_PULSE:
        c = 'd';
        toSleep = true;
        pulseMs = 80;
        sleepMs = 400;
        break;
    case BOT_L_PULSE:
        c = 'a';
        toSleep = true;
        pulseMs = 80;
        sleepMs = 400;
        break;
    }
     // << "Executing " << c << ", toSleep = " << toSleep;
    s.WriteByte(c);
    if(toSleep)
    {
        usleep(1000*pulseMs);
        s.WriteByte('O');
        usleep(1000*sleepMs);
    }
}

bool AlgoWorker::moveToPoint(CvPoint botPos, double botAngle, CvPoint dest)
{
    CvPoint dirVec = cvPoint(-botPos.x + dest.x, -botPos.y + dest.y);
    double dirAngle = atan2(dirVec.y, dirVec.x);
    double angleTowardPoint = dirAngle- botAngle;
    int dSq = distSq(botPos, dest);
    while(angleTowardPoint > CV_PI)
        angleTowardPoint -= 2*CV_PI;
    while(angleTowardPoint < -CV_PI)
        angleTowardPoint += 2*CV_PI;
    if(dSq > MIN_BOT_CUBE_DIST_COARSE_SQR)
    {
        if(angleTowardPoint > ANGLE_TOLERANCE_COARSE)
        {
            execCommand(BOT_RIGHT);
        }
        else if(angleTowardPoint < -ANGLE_TOLERANCE_COARSE)
        {
            execCommand(BOT_LEFT);
        } else
         execCommand(BOT_FORWARD);
    }
    else
    {
        if(dSq > MIN_BOT_CUBE_DIST_FINE_SQR)
        {

            if(angleTowardPoint > ANGLE_TOLERANCE_FINE)
            {
                execCommand(BOT_R_PULSE);
            }
            else if(angleTowardPoint < -ANGLE_TOLERANCE_FINE)
            {
                execCommand(BOT_L_PULSE);
            } else
                execCommand(BOT_F_PULSE);
        }
        else
        {
            execCommand(BOT_STOP);
            for (int i = 0; i < 5; ++i)
            {
              execCommand(BOT_LED_ON);
              usleep(100*1000);
              execCommand(BOT_LED_OFF);
              usleep(100*1000);
            }
            return false;
        }

//        qDebug() << distSq(bs.botPos, curDest) << " "<< MIN_BOT_CUBE_DIST*MIN_BOT_CUBE_DIST<<" "<< dirAngle << " " << bs.botAngle <<"l";
//        s.WriteByte('P');
//        for(int i=0; i<5; i++)
//        {
//            s.WriteByte('l');
//            usleep(200000);
//        }
//        botState = BOT_MOVETOCAMP;
    }
    return true;
}

void AlgoWorker::liftTrap()
{
    s.WriteByte('P');
    for(int i=0; i<12; i++)
    {
        s.WriteByte('u');
        usleep(200000);
    }
}

void AlgoWorker::lowerTrap()
{
    s.WriteByte('P');
    for(int i=0; i<8; i++)
    {
        s.WriteByte('l');
        usleep(200000);
    }
}

CubeData AlgoWorker::chooseBestResourceCube()
{
    vector<CubeData> allResourceCubes;
    for(int i=0; i<bs.woodCubes.size(); i++)
        allResourceCubes.push_back(bs.woodCubes[i]);
    for(int i=0; i<bs.silverCubes.size(); i++)
        allResourceCubes.push_back(bs.silverCubes[i]);
    for(int i=0; i<bs.goldCubes.size(); i++)
        allResourceCubes.push_back(bs.goldCubes[i]);
    if(!allResourceCubes.size())
    {
        return CubeData();
    }
    int minDistSq = distSq(resourceCubeDest, allResourceCubes[0].centre);
    CubeData closestCube = allResourceCubes[0];
    for(int i=1; i<allResourceCubes.size(); i++)
    {
        int tempDistSq = distSq(resourceCubeDest, allResourceCubes[i].centre);
        if(tempDistSq < minDistSq)
        {
            minDistSq = tempDistSq;
            closestCube = allResourceCubes[i];
        }
    }    
    return closestCube;
}

bool AlgoWorker::calcCubeInter(CubeData cube)
{
    if(cube.centre.x > 640/2 || cube.centre.y < 480/2)
        return true;
    return false;
}
