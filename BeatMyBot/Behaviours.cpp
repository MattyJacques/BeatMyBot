// Title        : Behaviours.cpp
// Purpose      : Calculates desired velocity for the bot
//

#include "Behaviours.h"
#include "rules.h"            // For game rule definitions
#include "staticmap.h"        // For InsideBlock() and NormalToSurface()
#include "Renderer.h"         // Debug drawing


Behaviours::Behaviours()
{ // Constructor initialises all variables within class to null etc

  seekSwitch = false;
  arriveSwitch = false;
  pursueSwitch = false;
  evadeSwitch = false;
  fleeSwitch = false;
  avoidWallsSwitch = true;
  followPathSwitch = true;
  domTarget = -1;

} // Behaviours()

void Behaviours::SetBehaviours(bool doSeek, bool doArrive, bool doPursue, 
                               bool doEvade, bool doFlee, bool doWalls, 
                               bool doPath)
{ // Sets all the various behaviours to true or false using the parameters
  // provided

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
{ // Takes in target position and returns desired velocity to reach the target

  Vector2D desiredVelocity = (targetPosition - botPosition).unitVector() 
                             * MAXBOTSPEED;

  return (desiredVelocity - botVelocity).unitVector() * MAXIMUMACCELERATION;

} // Seek()


Vector2D Behaviours::Arrive(Vector2D targetPosition, Vector2D botPosition,
                            Vector2D botVelocity)
{ // Returns a vector2D that can be used to arrive at a target position by 
  // slowing down the velocity of the bot 

  float speed = (targetPosition - botPosition).magnitude() / 5;

  // Check to make sure bots speed is not above max speed in rules
  if (speed > MAXBOTSPEED)
    speed = MAXBOTSPEED;

  Vector2D result = (targetPosition - botPosition).unitVector() * speed;

  return result - botVelocity;

} // Arrive()


Vector2D Behaviours::Pursue(Vector2D targetPosition, Vector2D targetVelocity,
                            Vector2D botPosition, Vector2D botVelocity)
{ // Returns a vector2D that can be used to pursue a moving target using the
  // targets position and velocity to work out an intercept

  float distance = (targetPosition - botPosition).magnitude();
  float time = distance / MAXBOTSPEED;

  targetPosition += targetVelocity * time;

  return Seek(targetPosition, botPosition, botVelocity);

} // Pursue()


Vector2D Behaviours::Evade() // NOT COMPLETED
{ // Takes in the target the bot is running from and provides a vector2D for
  // use in the Flee()

  Vector2D behaviourAccn;
  behaviourAccn.XValue = 1;
  behaviourAccn.YValue = 1;

  return behaviourAccn;

} // Evade()


Vector2D Behaviours::Flee() // NOT COMPLETED
{ // Takes in a vector2D and returns a velocity directly away from that point

  Vector2D behaviourAccn;
  behaviourAccn.XValue = 1;
  behaviourAccn.YValue = 1;

  return behaviourAccn;

} // Flee()


Vector2D Behaviours::WallAvoid(Vector2D botPosition, Vector2D botVelocity)
{ // Uses a circle2D as a collision box provided a velocity that can be used
  // the avoid nearby walls

  // Create circle for testing if going to collide
  Circle2D bigCol(botPosition, 100);      // Potential collision
  Circle2D smallCol(botPosition, 60);     // Immediate collision

  Vector2D test(botPosition.XValue + 50, botPosition.YValue + 50);
  Renderer::GetInstance()->DrawLine(botPosition, test);

  // Create a Vector2D for velocity and init to 0
  Vector2D desiredVelocity;
  desiredVelocity.set(0, 0);
  
  if (StaticMap::GetInstance()->IsInsideBlock(bigCol))
  { // If there is a block inside the col circle, create velocity in opposite
    // direction

    desiredVelocity = StaticMap::GetInstance()->GetNormalToSurface(bigCol);

    if (StaticMap::GetInstance()->IsInsideBlock(smallCol))
      desiredVelocity *= 120;
    else
      desiredVelocity *= 60;

  }

  return desiredVelocity;

} // WallAvoid()


// DONT LIKE THE LOOK, CHANGE LATER
Vector2D Behaviours::FollowPath(std::vector<Vector2D>* myPath, Vector2D botPosition, 
                                Vector2D botVelocity)
{ // Returns the result of Seek() to the next node on the path, has a check
  // to see if the bot can skip and node and seek straight to the one after

  Vector2D nextNode;

  if (!myPath->size() == 0)
  { // If there are Vector2Ds left in the path, figure out velocity

    nextNode = myPath->back();

    if (myPath->size() > 1)
    { // Check to see if the bot can skip a node in path

      if (StaticMap::GetInstance()->IsLineOfSight(botPosition, (*myPath)[myPath->size() - 2]))
      { // If we can see a two nodes in front, skip next node

        nextNode = (*myPath)[myPath->size() - 2];
        myPath->pop_back();

      }

      // Create circle of arrive check
      Circle2D arriveCircle(botPosition, 35);

      if (arriveCircle.Intersects(Point2D(nextNode)))
      { // Check if cord we want to be at is in circle, if so get next node

        myPath->pop_back();
        nextNode = myPath->back();

      }
      
    } // If size() > 1

  } // If path != 0

  // Return the velocity to the next node
	return Seek(nextNode, botPosition, botVelocity);

} // FollowPath()

Vector2D Behaviours::Accumulate(Vector2D targetPosition, 
                                Vector2D targetVelocity, Vector2D botPosition, 
                                Vector2D botVelocity, 
                                std::vector<Vector2D>* myPath)
{ // Accumulates all the vector2D velocitys from the other behaviours to
  // provide the real velcoity the bot should currently be following

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
