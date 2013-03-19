#include "beliefstate.h"
#include <math.h>

BeliefState::BeliefState()
{
    isBot = false;
    botPos = cvPoint(-1, -1);
    botAngle = 0;
}

void BeliefState::populateFromBlobData(vector<BlobData> bd)
{
    woodCubes = getColorCubes(WOOD_COLOR, bd);
    silverCubes = getColorCubes(SILVER_COLOR, bd);
    goldCubes = getColorCubes(GOLD_COLOR, bd);
    bombCubes = getColorCubes(BOMB_COLOR, bd);
    int foundFront = 0, foundBack=0;
    for(int i=0; i<bd.size(); i++)
    {
        if(bd[i].color == BOTFRONT_COLOR)
        {
            foundFront = 1;
//            qDebug() << "front found";
            botFront.x = bd[i].centre.x;
            botFront.y = bd[i].centre.y;
            break;
        }
    }
    for(int i=0; i<bd.size(); i++)
    {
        if(bd[i].color == BOTBACK_COLOR)
        {
            foundBack = 1;
//            qDebug() << "back found";
            botBack.x = bd[i].centre.x;
            botBack.y = bd[i].centre.y;
            break;
        }
    }
    if(!foundFront || !foundBack)
    {
        botPos = cvPoint(-1, -1);
        isBot = false;
        botAngle = 0;
        return;
    }
    isBot = true;
    botAngle = atan2(botFront.y - botBack.y, botFront.x - botBack.x);
    botPos = cvPoint((botFront.x + botBack.x)/2, (botFront.y + botBack.y)/2);
}

vector<CubeData> BeliefState::getColorCubes(Color color, vector<BlobData> &bd)
{
    vector <CubeData> ret;
    int id = 0;
    for(int i=0; i<bd.size(); i++)
    {
        if(bd[i].color == color)
            ret.push_back(CubeData(bd[i], id++));
    }
    return ret;
}
