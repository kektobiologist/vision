#include "blob.h"
#include "config.h"
#include "arena.h"
Blob::Blob(CvSize _size)
{
    numBlobs = 0;
    isDetected = 0;
    size = _size;
    visited = cvCreateImage(size, 8, 1);
    output  = cvCreateImage(size, 32, 1);
}
Blob::~Blob()
{
    cvReleaseImage(&visited);
    cvReleaseImage(&output);
}
void Blob::_detectBlobs(int i, int j, queue<Point2D> &q, IplImage *src, Color curColor)
{
    if(!(i>=0 && i<size.height && j>=0 && j<size.width))
        return;
    if(IMGDATA(visited, i, j, 0))
        return;
    IMGDATA(visited, i, j, 0) = 1;
    if((Color)IMGDATA(src, i, j, 0) == curColor)
    {
        IMGDATA(output, i, j, 0) = numBlobs;
        q.push(Point2D(i,j));
    }
}
void Blob::detectBlobs(IplImage *src, IplImage *zoneImage)//Detects blobs of all colors
{
    if(!src || (src->height != size.height && src->width != size.width) || src->nChannels != 1)
    {
        printf("Error! Input not matching/is NULL\n");
        return;
    }
    blobIDArr.clear();
    blobDataArr.clear();
    vector<int> notBlob;
    vector<int> blobSizes;
    int h = size.height;
    int w = size.width;
    for (int i = 0; i < h; ++i)
    {
        for (int j = 0; j < w; ++j)
        {
            IMGDATA(visited, i, j, 0) = IMGDATA(output, i, j, 0) = 0;
        }
    }
    numBlobs = 0;
    int i=0, j=0;
    while(i<h && j<w)
    {
        int flag = 0;
        for(;i<h;i++)
        {
            for(; j<w; ++j)
            {
                if(IMGDATA(visited, i, j, 0))
                    continue;
                IMGDATA(visited, i, j, 0) = 1;
                if((Color)IMGDATA(src, i, j, 0)!=UNDEF)
                {
                    flag = 1;
                    break;
                }
            }
            if(j == w)
                j = 0;
            if(flag)
                break;
        }
        Color curColor;
        if((Color)IMGDATA(src, i, j, 0)!=UNDEF)
        {
            curColor = (Color)IMGDATA(src, i, j, 0);
            // printf("newBlob\n");
            int sumx=0, sumy=0;
            int blobSize = 0;
            queue<Point2D> q;
            q.push(Point2D(i,j));
            IMGDATA(output, i, j, 0) = (++numBlobs);
            Point2D cur(i,j);
            int mini=h-1, maxi=0, minj=w-1, maxj=0;
            while(q.size())
            {
                cur = q.front();
                int it = cur.i;
                int jt = cur.j;
                sumx+= jt;
                sumy+= it;
                if(it<mini) mini=it; if(it>maxi) maxi=it;
                if(jt<minj) minj=jt; if(jt>maxj) maxj=jt;
                q.pop();
                blobSize++;
                for (int k = it-1; k <= it+1; ++k)
                {
                    for (int l = jt-1; l <= jt+1; ++l)
                    {
                        _detectBlobs(k, l, q, src, curColor);
                    }
                }
            }
            Zone tempZone = ZONE_UNDEF;
            int isBlobFlag = 1;
            if(blobSize < MINBLOBSIZE || blobSize > MAXBLOBSIZE)
            {
                // printf("%d not a blob!\n", numBlobs);
                isBlobFlag = 0;
//                notBlob.push_back(numBlobs);
            }
            else
            {
                if((curColor == WOOD_COLOR || curColor == SILVER_COLOR || curColor == GOLD_COLOR))
                {
                    tempZone = (Zone)IMGDATA(zoneImage, sumy/blobSize, sumx/blobSize, 0);
                    if(tempZone == ZONE_UNDEF || tempZone == ZONE_MINE_DEPOSIT)
                    {
                        isBlobFlag = 0;
//                        notBlob.push_back(numBlobs);
                    }
                }
                else if(curColor == BOMB_COLOR)
                {
                    tempZone = (Zone)IMGDATA(zoneImage, sumy/blobSize, sumx/blobSize, 0);
                    if(tempZone == ZONE_UNDEF || tempZone == ZONE_BOMB_DEPOSIT)
                    {
                        isBlobFlag = 0;
//                        notBlob.push_back(numBlobs);
                    }
                }
            }
            if(!isBlobFlag)
            {
                notBlob.push_back(numBlobs);
            }
            else
            {
                blobDataArr.push_back(BlobData(cvPoint(sumx/blobSize, sumy/blobSize), cvPoint(minj, mini), cvPoint(maxj, maxi), curColor));
                blobSizes.push_back(blobSize);
//				blobArr.push_back(newBlob);
                blobIDArr.push_back(numBlobs);
            }
//                else
//                {
//                    blobDataArr.push_back(BlobData(cvPoint(sumx/blobSize, sumy/blobSize), cvPoint(minj, mini), cvPoint(maxj, maxi), curColor));
//                    blobSizes.push_back(blobSize);
//    //				blobArr.push_back(newBlob);
//                    blobIDArr.push_back(numBlobs);
//                }
//            }
        }
    }
//    for (int i = 0; i < h; ++i)
//    {
//        for (int j = 0; j < w; ++j)
//        {
//            if(!IMGDATA(output, i, j, 0))
//                continue;
//            for (int k = 0; k < notBlob.size(); ++k)
//            {
//                if(IMGDATA(output, i, j, 0) == notBlob[k])
//                    IMGDATA(output, i, j, 0) = 0;
//            }
//        }
//    }
    //Bubble sorting by blobSize
    for (int i = 0; i < blobSizes.size(); ++i)
    {
        for (int j = i+1; j < blobSizes.size(); ++j)
        {
            if(blobSizes[i] < blobSizes[j])
            {
                swap(blobSizes[i], blobSizes[j]);
                swap(blobIDArr[i], blobIDArr[j]);
                swap(blobDataArr[i], blobDataArr[j]);
//				swap(blobArr[i], blobArr[j]);
            }
        }
    }
    numBlobs = blobIDArr.size();
    isDetected = 1;
}
