#ifndef ARENA_H
#define ARENA_H
#include <highgui/highgui.hpp>
#include <core/core.hpp>
#include <opencv.hpp>
#include <QtCore>
#define ARENA_CONFIG "arena-config"
static const int MAX_MOUSE_POINT_DIST_SQR = 20*20;
enum Zone{
    ZONE_UNDEF,
    ZONE_START,
    ZONE_MINE,
    ZONE_BOMB,
    ZONE_MINE_DEPOSIT,// resource cubes won't be searched in this region.
    ZONE_BOMB_DEPOSIT,// bomb cubes won't be searched in this region.
    ZONE_MAX_ZONE
};

class Arena
{
public:
    Arena();
    ~Arena();
    void drawArenaDisplay(IplImage *img);
    IplImage* getZoneImage(){return zoneImage;}
    void updateOnMouseClick(int x, int y);
    void updateOnMouseDrag(int x, int y);
    CvPoint getBombDrop(){ return bombDrop;}
    CvPoint getMineDrop(){ return mineDrop;}
    CvPoint getStartCorner(){return startCorner;}
private:
    int px, py;
    IplImage *zoneImage;
    CvPoint arenaTL;
    CvPoint arenaBR;
    CvPoint mineTR;// resource cubes won't be searched in this region.
    CvPoint bombBL;// bomb cubes won't be searched in this region.
    CvPoint mineDrop;// resource cubes dropped here.
    CvPoint bombDrop;// bomb cubes dropped here.
    CvPoint startCorner;// corner of start zone
    // For assistance:
    CvPoint *curPoint;
    CvPoint *listOfPoints[7]; // list of all real point variables! please keep updating on addition!
    int sizeOfList; //remember = 7!
    CvFont *ft;
    bool load();
    void save();
    void createZoneImage();
};

#endif // ARENA_H
