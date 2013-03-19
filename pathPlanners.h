#ifndef PATH_PLANNER_H
#define PATH_PLANNER_H

#include <vector>
#include <list>
#include "geometry.hpp"
#include "fieldConfig.h"
#define MAX_OBSTACLES (HomeTeam::SIZE + AwayTeam::SIZE + 1)

// Forward Declarations
namespace Strategy
{
  template <class T> class KDTree;
}
#if GR_SIM_COMM
# include "grSimComm.h"
#elif SIM_COMM
# include "simComm.h"
#elif SSL_COMM
# include "sslComm.h"
#elif FIRA_COMM
# include "fira_comm.h"
#else
# error Macro for Comm class not defined
#endif



#if GR_SIM_COMM
  typedef HAL::GrSimComm CommType;
#elif SIM_COMM
  typedef HAL::SimComm   CommType;
#elif SSL_COMM
  typedef HAL::SSLComm   CommType;
#elif FIRA_COMM
  typedef HAL::FIRAComm  CommType;
#else
# error Macro for Comm class not defined
#endif

namespace Strategy
{
  typedef struct
  {
    float x, y;
    float radius;
  } obstacle;

  class ERRT
  {
  public:
    typedef struct
    {
      Point2D<int> center;
      int radius;
    } obstacle;

  public:
    static const int WAYPOINT_CACHE_SIZE = 50;
    const float StepLength;

    const float GoalProb;
    const float WaypointCacheProb;

    int          waypointCacheEntries;
    Point2D<int> waypointCache[WAYPOINT_CACHE_SIZE];

    Point2D<int> chooseTarget(const Point2D<int>& goal);

    Point2D<int> getNearest(KDTree<int>& tree, const Point2D<int>& target);

    Point2D<int> extend(const std::vector<obstacle>& obs,
                        const Point2D<int>&          nearest,
                        const Point2D<int>&          target,
                        const Point2D<int>&          initial);

    // Returns a random point anywhere on the playing field
    Point2D<int> randomState(void) const;

    // Detects if a point collides with any of the obstacles
    inline bool collides(const Point2D<int>& point, const std::vector<obstacle>& obs)
    {
      int size = obs.size();
      for (int i = 0; i < size; ++i)
      {
        if (intersects(point, obs[i].center, obs[i].radius))
          return true;
      }
      return false;
    }

    // Detects if a line segment whose ends are point1 and point2 collides with any of the obstacles
    inline bool collides(const Point2D<int>&          point1,
                         const Point2D<int>&          point2,
                         const std::vector<obstacle>& obs)
    {
      int size = obs.size();
      for (int i = 0; i < size; ++i)
      {
        if (intersects(point1, point2, obs[i].center, obs[i].radius))
          return true;
      }
      return false;
    }

  public:
    ERRT(float GoalProb, float WaypointCacheProb);

    ~ERRT();

    bool plan(const Point2D<int>&          start,
              const Point2D<int>&          goal,
              const std::vector<obstacle>& obs,
              const int                    goalThreshold,
              std::list<Point2D<int> >&    waypoints);
              
    void smoothPath(std::list<Point2D<int> >& waypoints, const std::vector<obstacle>& obs);
  }; // class ERRT

  class MergeSCurve
  {
  private:
    static const float x_min;
    static const float x_max;
    static const float y_min;
    static const float y_max;

    obstacle* Obstacles;
    int nObstacles;
    int nBotID;

  public:
    MergeSCurve();
    ~MergeSCurve();

    bool plan(const Vector2D<int>& initial,
              const Vector2D<int>& final,
              Vector2D<int> *pt1,
              Vector2D<int> *pt2,
              obstacle *obstacles,
              int obstacle_count,
              int current_id,
              bool teamBlue);

  private:
    int             MergeObstacles();
    void            ConnectObstacles(bool *Connected, int nCurrentObstacle);
    int             ifInObstacle(Vector2D<int> point, obstacle *obs, int nObs, int closest);
    int             Connect(Vector2D<int> Start, Vector2D<int> End, obstacle* obs, int nObs, int BotID, int *nearest, bool TeamBlue);
    Vector2D<int> getNextPt(Vector2D<int> initial, int closest, Vector2D<int> final, obstacle *obstacles, int nObs, int BotID, bool teamBlue);
    Vector2D<int> getNextPtRec(Vector2D<int> initial, int lastClosest, int closest, obstacle *obstacles, int nObs);
  }; // class MergeSCurve

  class NaivePathPlanner
  {
  private:
    static const float x_min;
    static const float x_max;
    static const float y_min;
    static const float y_max;

    float            threshold;
    float            startx, starty;
    int              total_node;
    float length;


  public:
    NaivePathPlanner();
    ~NaivePathPlanner();

    bool plan(Vector2D<float> initial, Vector2D<float> final, Vector2D<float> *pt1, Vector2D<float> *pt2, obstacle *obstacles, int nObs, int current_id, bool teamBlue);


  private:
    int Connect(Vector2D<float> Start, Vector2D<float> End, obstacle* obs, int nObs, int BotID, int *nearest, bool TeamBlue);

  }; // class NaivePathPlanner

  class SCurve
  {
  private:
    static const float x_min;
    static const float x_max;
    static const float y_min;
    static const float y_max;

    float              threshold;
    float              startx, starty;
    int                total_node;
    float              length;

  public:
    SCurve();

    ~SCurve();

    bool plan(Vector2D<float> initial, Vector2D<float> final, Vector2D<float> *pt1, Vector2D<float> *pt2, obstacle *obstacles, int obstacle_count, int current_id, bool teamBlue);

  private:
    int ifInObstacle(Vector2D<float> point, obstacle *obs, int nObs, int closest);
    int Connect(Vector2D<float> Start, Vector2D<float> End, obstacle* obs, int nObs, int BotID, int *nearest, bool TeamBlue);
    Vector2D<float> getNextPt(Vector2D<float> initial, int nClosest, Vector2D<float> final, obstacle *obstacles, int nObs, int BotID, bool teamBlue);
    Vector2D<float> getNextPtRec(Vector2D<float> initial, int lastClosest, int closest, obstacle *obstacles, int nObs);
  }; // class SCurve
  class LocalAvoidance
	{
	private:
		float phiStar;
		float theta;
		float phi;
		float dist;
		float alpha, beta;
		float thetaD, thetaDD, delta;
		float r, t;
		float profileFactor, velMod;
	public:
		LocalAvoidance();
		~LocalAvoidance();
		bool plan(Vector2D<int> initial, Vector2D<float> velocity, Vector2D<int> final, std::vector<obstacle> obstacles, int current_id, bool teamBlue, float curSlope, float finalSlope, float &t, float &r, CommType *comm = NULL, int clearance = CLEARANCE_PATH_PLANNER);
	private:	
		bool rayCasting(Vector2D<int> p1, Vector2D<int> p2, obstacle *obs);
		bool mergeObstacles(std::vector<obstacle> obstacles);
	}; //Class LocalAvoidance
}

#endif // PATH_PLANNER_H
