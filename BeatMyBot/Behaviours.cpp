#include "Behaviours.h"
#include "rules.h"
#include "staticmap.h"


Behaviours::Behaviours()
{
  seekSwitch = false;
  arriveSwitch = false;
  pursueSwitch = false;
  evadeSwitch = false;
  fleeSwitch = false;
  avoidWallsSwitch = true;
  followPathSwitch = true;

} // Behaviours()

void Behaviours::SetBehaviours(bool doSeek, bool doArrive, bool doPursue, 
                               bool doEvade, bool doFlee, bool doWalls, 
                               bool doPath)
{

  seekSwitch = doSeek;
  arriveSwitch = doArrive;
  pursueSwitch = doPursue;
  evadeSwitch = doEvade;
  fleeSwitch = doFlee;
  avoidWallsSwitch = doWalls;
  followPathSwitch = doPath;

} // SetBehaviours()


Vector2D Behaviours::Seek(Vector2D targetPosition, Vector2D botPosition,
                          Vector2D botVelocity)
{

  Vector2D desiredVelocity = (targetPosition - botPosition).unitVector() 
                             * MAXBOTSPEED;

  return (desiredVelocity - botVelocity).unitVector() * MAXIMUMACCELERATION;

} // Seek()


Vector2D Behaviours::Arrive(Vector2D targetPosition, Vector2D botPosition,
                            Vector2D botVelocity)
{

  float speed = (targetPosition - botPosition).magnitude() / 5;

  if (speed > MAXBOTSPEED)
    speed = MAXBOTSPEED;

  Vector2D result = (targetPosition - botPosition).unitVector() * speed;

  return result - botVelocity;

} // Arrive()


Vector2D Behaviours::Pursue(Vector2D targetPosition, Vector2D targetVelocity,
                            Vector2D botPosition, Vector2D botVelocity)
{
  float distance = (targetPosition - botPosition).magnitude();
  float time = distance / MAXBOTSPEED;

  targetPosition += targetVelocity * time;

  return Seek(targetPosition, botPosition, botVelocity);
} // Pursue()


Vector2D Behaviours::Evade() // NOT COMPLETED
{
  Vector2D behaviourAccn;
  behaviourAccn.XValue = 1;
  behaviourAccn.YValue = 1;

  return behaviourAccn;
} // Evade()


Vector2D Behaviours::Flee() // NOT COMPLETED
{
  Vector2D behaviourAccn;
  behaviourAccn.XValue = 1;
  behaviourAccn.YValue = 1;

  return behaviourAccn;
} // Flee()


Vector2D Behaviours::WallAvoid(Vector2D botPosition, Vector2D botVelocity)
{
  Circle2D colCircle(botPosition, 110);
  Vector2D desiredVelocity;
  desiredVelocity.set(0, 0);
  
  if (StaticMap::GetInstance()->IsInsideBlock(colCircle))
  {
    desiredVelocity = StaticMap::GetInstance()->GetNormalToSurface(colCircle);
    desiredVelocity *= 80;
  }

  return desiredVelocity;

} // WallAvoid()


// DONT LIKE THE LOOK, CHANGE LATER
Vector2D Behaviours::FollowPath(std::vector<Vector2D> &myPath, Vector2D botPosition, 
                                Vector2D botVelocity)
{ 
  Vector2D nextNode;

  if (!myPath.empty())
  {
    nextNode = myPath.back();

    if (myPath.size() > 1)
    { // Check to see if the bot can skip a node in path

      if (StaticMap::GetInstance()->IsLineOfSight(botPosition, myPath[myPath.size() - 2]))
        nextNode = myPath[myPath.size() - 2];

      Circle2D arriveCircle(botPosition, 20);
      if (arriveCircle.Intersects(Point2D(nextNode)))
      {
        myPath.pop_back();
        nextNode = myPath.back();
      }
    }
  }

	return Seek(nextNode, botPosition, botVelocity);

} // FollowPath()

Vector2D Behaviours::Accumulate(Vector2D targetPosition, 
                                Vector2D targetVelocity, Vector2D botPosition, 
                                Vector2D botVelocity, 
                                std::vector<Vector2D> &myPath)
{
  Vector2D acceleration;

  if (seekSwitch)
    acceleration += Seek(targetPosition, botPosition, botVelocity);

  if (arriveSwitch)
    acceleration += Arrive(targetPosition, botPosition, botVelocity);

  if (pursueSwitch)
    acceleration += Pursue(targetPosition, targetVelocity, botPosition, 
                           botVelocity);

  if (evadeSwitch)
    acceleration += Evade();

  if (fleeSwitch)
    acceleration += Flee();

  if (avoidWallsSwitch)
    acceleration += WallAvoid(botPosition, botVelocity);

  if (followPathSwitch)
    acceleration += FollowPath(myPath, botPosition, botVelocity);


  return acceleration;

} // Accumulate()
