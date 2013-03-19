#include "arena.h"
#include <stdio.h>
#include <iostream>
#include "config.h"
#ifndef IMGDATA
#define IMGDATA(image,i,j,k) (((uchar*)image->imageData)[(i)*(image->widthStep) + (j)*(image->nChannels) + (k)])
#endif
//int distSq(CvPoint, CvPoint);
Arena::Arena()
{
    zoneImage = cvCreateImage(cvSize(640, 480), 8, 1);
    for(int i=0; i< 480; i++)
    {
        for(int j=0; j<640; j++)
        {
            IMGDATA(zoneImage, i, j, 0) = ZONE_UNDEF;
        }
    }
    if(!load())
    {
        arenaTL = cvPoint(50, 50);
        arenaBR = cvPoint (640-50, 480-50);
        mineTR = cvPoint(100, 480-100);
        bombBL = cvPoint(640-100, 100);
        mineDrop = cvPoint(75, 480-75);
        bombDrop = cvPoint(640-75, 75);
        startCorner = cvPoint(200, 480-200);
    }
    curPoint = NULL;
    px = 0;
    py = 0;
    ft = new CvFont;
//    ft->font_face = CV_FONT_NORMAL;
//    ft->ascii = true;
    ft->color = cvScalar(255,255,255);
    ft->hscale = 10;
    ft->vscale = 10;
    createZoneImage();
}

Arena::~Arena()
{
    save();
    cvReleaseImage(&zoneImage);
}

void Arena::drawArenaDisplay(IplImage *img)
{
    if(!img)
    {
        qDebug() << "Arena given NULL Image!";
        return;
    }
    if(img->height != 480 || img->width != 640 || img->nChannels != 3)
    {
        qDebug() << "Arena given mis-sized image!";
        return;
    }
    cvRectangle(img, arenaTL, arenaBR, cvScalar(255, 255, 0));
    CvPoint arenaBL = cvPoint(arenaTL.x, arenaBR.y);
    CvPoint arenaTR = cvPoint(arenaBR.x, arenaTL.y);
    cvRectangle(img, arenaBL, mineTR, cvScalar(0,255,255));
    cvRectangle(img, bombBL, arenaTR, cvScalar(255,0,255));

    cvCircle(img, arenaTL, 5, cvScalar(255, 255, 0), -1);
    cvCircle(img, arenaBR, 5, cvScalar(255, 255, 0), -1);
    cvCircle(img, mineTR, 5, cvScalar(0, 255, 255), -1);
    cvCircle(img, bombBL, 5, cvScalar(255, 0, 255), -1);
    cvCircle(img, mineDrop, 5, cvScalar(0, 255, 255), 1);
    cvCircle(img, bombDrop, 5, cvScalar(255, 0, 255), 1);
    cvCircle(img, startCorner,5, cvScalar(255,255,255), 1);
//    cvAddText(img, "ArenaTL", cvPoint(arenaTL.x-10, arenaTL.y-10), ft);
//    cvAddText(img ,"ArenaBR", cvPoint(arenaBR.x+10, arenaBR.y+10), ft);
//    cvAddText(img ,"MineTR", cvPoint(mineTR.x+10, mineTR.y-10), ft);
//    cvAddText(img ,"BombBL", cvPoint(bombBL.x-10, bombBL.y+10), ft);
}

void Arena::updateOnMouseClick(int x, int y)
{
    int minDistSq = distSq(cvPoint(x,y), *listOfPoints[0]);
    int minIndex = 0;
    for(int i=1; i<sizeOfList; i++)
    {
        int tempDistSq = distSq(cvPoint(x,y), *listOfPoints[i]);
        if(tempDistSq < minDistSq)
        {
           minDistSq = tempDistSq;
           minIndex = i;
        }
    }
    if(minDistSq <= MAX_MOUSE_POINT_DIST_SQR)
    {
        curPoint = listOfPoints[minIndex];
    }
    else
    {
        curPoint = NULL;
    }
    px = x;
    py = y;
}

void Arena::updateOnMouseDrag(int x, int y)
{
    if(!curPoint)
        return;
    int dx = x-px;
    int dy = y-py;
    px = x;
    py = y;
    curPoint->x += dx;
    curPoint->y += dy;
    if(curPoint->x >= 640)
        curPoint->x = 640-1;
    else if(curPoint->x < 0)
        curPoint->x = 0;

    if(curPoint->y >= 480)
        curPoint->y = 480-1;
    else if(curPoint->y < 0)
        curPoint->y = 0;

    createZoneImage();
}

bool Arena::load()
{
    listOfPoints[0] = &arenaTL;
    listOfPoints[1] = &arenaBR;
    listOfPoints[2] = &mineTR;
    listOfPoints[3] = &bombBL;
    listOfPoints[4] = &mineDrop;
    listOfPoints[5] = &bombDrop;
    listOfPoints[6] = &startCorner;
    sizeOfList = 7;
    FILE *f = fopen(ARENA_CONFIG, "r");
    if(!f)
    {
        qDebug() << "Could'nt load arena data!";
        return false;
    }
    for(int i=0; i<sizeOfList; i++)
    {
        if(!fscanf(f, "%d %d ", &listOfPoints[i]->x, &listOfPoints[i]->y))
        {
            qDebug() <<"Error loading?";
            fclose(f);
            return false;
        }
    }
    fclose(f);
    return true;
}

void Arena::save()
{
    FILE *f = fopen(ARENA_CONFIG, "w");
    if(!f)
    {
        qDebug() << "Could'nt save arena data!";
        return;
    }
    for(int i=0; i<sizeOfList; i++)
        fprintf(f, "%d %d ", listOfPoints[i]->x, listOfPoints[i]->y);
    fprintf(f, "\n");
    qDebug() << "Arena data saved.";
    fclose(f);
}

void Arena::createZoneImage()
{
    if(!zoneImage)
    {
        qDebug() << "zoneImage not init..";
        return;
    }
    for(int i=0; i< 480; i++)
    {
        for(int j=0; j<640; j++)
        {
            IMGDATA(zoneImage, i, j, 0) = ZONE_UNDEF;
        }
    }
    CvPoint arenaBL = cvPoint(arenaTL.x, arenaBR.y);
    CvPoint arenaTR = cvPoint(arenaBR.x, arenaTL.y);
    cvRectangle(zoneImage, arenaTL, arenaBR, cvScalar(ZONE_MINE), -1);
    cvRectangle(zoneImage, mineTR, arenaBL, cvScalar(ZONE_MINE_DEPOSIT), -1);
    cvRectangle(zoneImage, bombBL, arenaTR, cvScalar(ZONE_BOMB_DEPOSIT), -1);
}
