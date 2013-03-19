#ifndef ALGOWORKER_H
#define ALGOWORKER_H

#include <QObject>
#include <QtCore>
#include <QThread>
#include <QMutex>
#include <QTimer>
#include "beliefstate.h"
#include "serial.h"
enum BotState{
    BOT_START,
    BOT_SEARCH_RESOURCE,
    BOT_SEARCH_BOMB,
    BOT_MOVETOCUBE,
    BOT_MOVETOINTER,
    BOT_MOVETOCAMP,
    BOT_FINISHED,
    BOT_MAX_STATES
};
enum BotCommand{
    BOT_FORWARD,
    BOT_BACKWARD,
    BOT_LEFT,
    BOT_RIGHT,
    BOT_STOP,
    BOT_UP,
    BOT_DOWN,
    BOT_F_PULSE,
    BOT_B_PULSE,
    BOT_L_PULSE,
    BOT_R_PULSE,
    MAX_BOTCOMMAND
};
enum CubeType{
    CUBE_UNDEF,
    CUBE_WOOD,
    CUBE_SILVER,
    CUBE_GOLD,
    CUBE_BOMB,
    MAX_CUBETYPE
};

class AlgoWorker : public QObject
{
    Q_OBJECT
public:
    explicit AlgoWorker(QObject *parent = 0);
    void setup(QThread *cThread, QMutex *_bsMutex, BeliefState *_bs);
signals:
    
public slots:
    void onEntry();
    void onEnd();
    void onTimeout();
private:
    int numWood, numSilver, numGold, numBombs;
    CvPoint resourceCubeDest;
    CvPoint bombCubeDest;
    CvPoint startCorner;
    HAL::Serial s;
    BotState botState;
    QThread *myThread;
    QMutex *bsMutex;
    QTimer *timer;
    BeliefState *bsCam;
    BeliefState bs;   
    CvPoint curDest;
    CubeData curCube;
    bool isInterCube;
    bool isInterCubeCaptured;
    void execCommand(BotCommand com);
    bool moveToPoint(CvPoint botPos, double botAngle, CvPoint dest);
    void liftTrap();
    void lowerTrap();
    CubeData chooseBestResourceCube();// returns cvPoint(-1,-1) if no cube.
    bool calcCubeInter(CubeData cube);
};

#endif // ALGOWORKER_H
