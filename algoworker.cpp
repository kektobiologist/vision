#include "algoworker.h"
#include <QCoreApplication>
#include "config.h"
#include "serial.h"
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
    s.WriteByte('P');
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
    s.WriteByte('P');
    s.WriteByte('p');
    botState = BOT_START;
    s.Close();
    qDebug() << "Algo Worker stopped.";
}

void AlgoWorker::onTimeout()
{
    bsMutex->lock();
    bs = *bsCam;
    bsMutex->unlock();
//    qDebug() << bs.botPos.x << "/// " << bs.botPos.y;
    //Do work.
//    qDebug() << "Inter = " << bs.startCorner.x << ",  " << bs.startCorner.y;
//    qDebug() << "mine deposit = " << bs.resourceDepositPoint.x << ", " << bs.resourceDepositPoint.y;
//    qDebug() << "bomb deposit = "  << bs.bombDepositPoint.x << ", " << bs.bombDepositPoint.y;
    vector <CubeData> cubes;
    resourceCubeDest = bs.resourceDepositPoint;
    bombCubeDest = bs.bombDepositPoint;
    startCorner = bs.startCorner;
    switch(botState)
    {
    case BOT_START:
        qDebug() << "Bot Start!";
    case BOT_SEARCH_RESOURCE:
        // Experimental : use chooseBestResourceCube() ?
          curCube = chooseBestResourceCube();
          if(!(curCube.color == WOOD_COLOR || curCube.color == SILVER_COLOR || curCube.color == GOLD_COLOR))
          {
            qDebug() << "no resource cubes left.. going after bombs";
            curCube = CubeData();
            botState = BOT_SEARCH_BOMB;
            break;
          }
          isInterCube = calcCubeInter(curCube);
          if(isInterCube)
          {
              qDebug() << "move to inter";
              curDest = startCorner;
              isInterCubeCaptured = false;
              botState = BOT_MOVETOINTER;
              break;
          }
          curDest = curCube.centre;
          qDebug() << "going after resource cube at" << curDest.x << ", " << curDest.y;
          botState = BOT_MOVETOCUBE;
          break;
    case BOT_SEARCH_BOMB:
        isInterCubeCaptured = false;
        if(!bs.bombCubes.size())
        {
            qDebug() << "no bomb cubes found... returning.";
            return;
        }
        isInterCube = false;
        curCube = bs.bombCubes[0];
        curDest = curCube.centre;
        botState = BOT_MOVETOCUBE;
        break;
    case BOT_MOVETOINTER:
        if(!moveToPoint(bs.botPos, bs.botAngle, curDest))
        {
            //reached inter
            if(isInterCubeCaptured)
            {
                curDest = resourceCubeDest;
                botState = BOT_MOVETOCAMP;
                isInterCubeCaptured = false;
                break;
            }
            else
            {
                curDest = curCube.centre;
                botState = BOT_MOVETOCUBE;
                break;
            }
        }
        break;
    case BOT_MOVETOCUBE:        
        if(!moveToPoint(bs.botPos, bs.botAngle, curDest))
        {
            execCommand(BOT_DOWN);
            if(isInterCube)
            {
                curDest = startCorner;
                botState = BOT_MOVETOINTER;
                isInterCubeCaptured = true;
                break;
            }
            if(curCube.color == BOMB_COLOR)
                curDest = bombCubeDest;
            else if(curCube.color == WOOD_COLOR || curCube.color == SILVER_COLOR || curCube.color == GOLD_COLOR)
                curDest = resourceCubeDest;
            else
                qDebug()<< "Don't know current color!";
            botState = BOT_MOVETOCAMP;
        }
        break;
    case BOT_MOVETOCAMP:
        if(!moveToPoint(bs.botPos, bs.botAngle, curDest))
        {
            execCommand(BOT_UP);
            execCommand(BOT_FORWARD);
            usleep(1000*1000);
            execCommand(BOT_BACKWARD);
            usleep(1000*1000);
            execCommand(BOT_STOP);
            switch(curCube.color)
            {
            case WOOD_COLOR: numWood++; break;
            case SILVER_COLOR: numSilver++; break;
            case GOLD_COLOR: numGold++; break;
            case BOMB_COLOR: numBombs++; break;
            default: qDebug() << "Eror! don't know color of deposited cube!";
            }
            botState = BOT_START;
        }
        break;
    }
//    emit currentStateReady(botState);
//    qDebug() << "x, y = " << bs.getBotPos().x << ", " << bs.getBotPos().y;
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
    case BOT_UP:
        liftTrap();
        break;
    case BOT_DOWN:
        lowerTrap();
        break;
    case BOT_STOP:
        c = 'P';
        break;
    case BOT_F_PULSE:
        c = 'W';
        toSleep = true;
        pulseMs = 100;
        sleepMs = 400;
        break;
    case BOT_B_PULSE:
        c = 'S';
        toSleep = true;
        pulseMs = 100;
        sleepMs = 400;
        break;
    case BOT_R_PULSE:
        c = 'D';
        toSleep = true;
        pulseMs = 100;
        sleepMs = 1000;
        break;
    case BOT_L_PULSE:
        c = 'A';
        toSleep = true;
        pulseMs = 100;
        sleepMs = 1000;
        break;
    }
    qDebug() << "Executing " << c << ", toSleep = " << toSleep;
    s.WriteByte(c);
    if(toSleep)
    {
        usleep(1000*pulseMs);
        s.WriteByte('P');
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
    if(angleTowardPoint > ANGLE_TOLERANCE_COARSE)
    {
        execCommand(BOT_RIGHT);
    }
    else if(angleTowardPoint < -ANGLE_TOLERANCE_COARSE)
    {
        execCommand(BOT_LEFT);
    }
    else if(dSq > MIN_BOT_CUBE_DIST_COARSE_SQR)
    {
        execCommand(BOT_FORWARD);
    }
    else
    {
        if(angleTowardPoint > ANGLE_TOLERANCE_FINE)
        {
            execCommand(BOT_R_PULSE);
        }
        else if(angleTowardPoint < -ANGLE_TOLERANCE_FINE)
        {
            execCommand(BOT_L_PULSE);
        }
        else
        {
            if(dSq > MIN_BOT_CUBE_DIST_FINE_SQR)
            {
                execCommand(BOT_F_PULSE);
            }
            else
            {
                execCommand(BOT_STOP);
                return false;
            }
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
