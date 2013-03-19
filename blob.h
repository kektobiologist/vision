#ifndef BLOB_H
#define BLOB_H

#include <stdio.h>
#include <highgui/highgui.hpp>
#include <queue>
#include <vector>
using namespace std;
#ifndef IMGDATA
#define IMGDATA(image,i,j,k) ((image->imageData)[(i)*(image->widthStep) + (j)*(image->nChannels) + (k)])
#endif
#define MINBLOBSIZE 50
#define MAXBLOBSIZE 50000 //around 200 should be ok for 4x4 cubes
#include "lut3d.h"
struct Point2D{
    int i, j;
    Point2D(int i, int j):i(i), j(j) {}
};
struct BlobData{
    CvPoint centre;
    CvPoint p1, p2;
    Color color;
    BlobData(){
        centre = cvPoint(0,0);
        p1 = centre;
        p2 = centre;
        color = WHITE;
    }
    BlobData(CvPoint centre, CvPoint p1, CvPoint p2, Color color): centre(centre), p1(p1), p2(p2), color(color){}
};

// template <class T>
// void swap(T &a, T &b)
// {
// 	T temp = a;
// 	a = b;
// 	b = temp;
// }
class Blob
{
public:
    Blob(CvSize _size);
    Blob(){size = cvSize(0,0);}
    ~Blob();
    void detectBlobs(IplImage *src, IplImage *zoneImage);//Detects blobs of all colors
    int getNumBlobs() {return numBlobs;}
    vector<int> getBlobIDArr(){return blobIDArr;}
    vector<IplImage*> getBlobArr(){return blobArr;}
    vector<BlobData> getBlobDataArr(){return blobDataArr;}
    IplImage* getBlobImage(){ return isDetected ? output : NULL;}
private:
    void _detectBlobs(int i, int j, queue<Point2D> &q, IplImage *src, Color curColor);
    vector<int> blobIDArr;
    vector<BlobData> blobDataArr;
    vector<IplImage*> blobArr;
    int numBlobs;
    int isDetected;
    CvSize size;
    IplImage *visited;
    IplImage *output;
};

#endif // BLOB_H
