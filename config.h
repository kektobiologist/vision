#ifndef CONFIG_H
#define CONFIG_H
#include <highgui/highgui.hpp>
#include <core/core.hpp>
#include <opencv.hpp>
static const int MIN_BOT_CUBE_DIST_COARSE = 50;
static const int MIN_BOT_CUBE_DIST_COARSE_SQR = MIN_BOT_CUBE_DIST_COARSE*MIN_BOT_CUBE_DIST_COARSE;
static const int MIN_BOT_CUBE_DIST_FINE = 40;
static const int MIN_BOT_CUBE_DIST_FINE_SQR = MIN_BOT_CUBE_DIST_FINE*MIN_BOT_CUBE_DIST_FINE;
static const int MIN_CUBE_DEST_DIST_COARSE = 50;
static const int MIN_CUBE_DEST_DIST_COARSE_SQR = MIN_CUBE_DEST_DIST_COARSE*MIN_CUBE_DEST_DIST_COARSE;
static const int MIN_CUBE_DEST_DIST_FINE = 50;
static const int MIN_CUBE_DEST_DIST_FINE_SQR = MIN_CUBE_DEST_DIST_FINE*MIN_CUBE_DEST_DIST_FINE;
static const double ANGLE_TOLERANCE_COARSE = 0.4;
static const double ANGLE_TOLERANCE_FINE = 0.15;


#define WOOD_COLOR RED
#define SILVER_COLOR CYAN
#define GOLD_COLOR YELLOW
#define BOMB_COLOR PINK

#define BOTFRONT_COLOR BLUE
#define BOTBACK_COLOR ORANGE

#define SERIAL_PORT "/dev/rfcomm1" //indicatory only, not yet used
#define WITHIN_TOL(a,b,tol) ((a-b) < tol && (a-b) > -tol)

static int distSq(CvPoint p1, CvPoint p2)
{
    return ((p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y));
}

#endif // CONFIG_H
