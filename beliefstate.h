#ifndef BELIEFSTATE_H
#define BELIEFSTATE_H
#include "arena.h"
#include "blob.h"
#include "lut3d.h"
#include <vector>
#include "config.h"

using namespace std;
struct CubeData{
    CvPoint centre;
    Color color;
    int id;
    CubeData(){
        centre = cvPoint(0,0);
        color = UNDEF;
        id = -1;
    }
    CubeData(BlobData b, int _id)
    {
        centre = b.centre;
        color = b.color;
        id = _id;
    }
};

class BeliefState
{
public:
    BeliefState();
    void populateFromBlobData(vector<BlobData> bd);
    CvPoint getBotPos(){return botPos;}
    double getBotAngle(){return botAngle;}
    vector <CubeData> getWoodCubes(){return woodCubes;}
    vector <CubeData> getSilverCubes(){return silverCubes;}
    vector <CubeData> getGoldCubes(){return goldCubes;}
    vector <CubeData> getBombCubes(){return bombCubes;}
    bool isBot;
    CvPoint botPos;
    CvPoint botFront;
    CvPoint botBack;
    double botAngle;
    vector <CubeData> woodCubes;
    vector <CubeData> silverCubes;
    vector <CubeData> goldCubes;
    vector <CubeData> bombCubes;
    CvPoint resourceDepositPoint;
    CvPoint bombDepositPoint;
    CvPoint startCorner;
private:
    vector <CubeData> getColorCubes(Color color, vector<BlobData> &bd);
};

#endif // BELIEFSTATE_H
