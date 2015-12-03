#include "Behaviours.h"
#include "rules.h"
#include "staticmap.h"


Behaviours* Behaviours::pInstance = nullptr;


Behaviours::Behaviours()
{
  seekOn = false;
  pathfindOn = false;
  avoidWallsOn = false;
}

Behaviours* Behaviours::GetInstance()
{
  if (!pInstance)
    pInstance = new Behaviours();

  return pInstance;
}

Vector2D Behaviours::Seek(Vector2D targetPosition, Vector2D botPosition,
                          Vector2D botVelocity)
{

  Vector2D desiredVelocity = (targetPosition - botPosition).unitVector() 
                             * MAXBOTSPEED;

  return (desiredVelocity - botVelocity).unitVector() * MAXIMUMACCELERATION;

}

Vector2D Behaviours::Arrive(Vector2D targetPosition, Vector2D botPosition,
                            Vector2D botVelocity)
{

  float speed = (targetPosition - botPosition).magnitude() / 5;

  if (speed > MAXBOTSPEED)
    speed = MAXBOTSPEED;

  Vector2D result = (targetPosition - botPosition).unitVector() * speed;

  return result - botVelocity;

}

Vector2D Behaviours::Pursue(Vector2D targetPosition, Vector2D targetVelocity,
                            Vector2D botPosition, Vector2D botVelocity)
{
  float distance = (targetPosition - botPosition).magnitude();
  float time = distance / MAXBOTSPEED;

  targetPosition += targetVelocity * time;

  return Seek(targetPosition, botPosition, botVelocity);
}

Vector2D Behaviours::Evade() // NOT COMPLETED
{
  Vector2D behaviourAccn;
  behaviourAccn.XValue = 1;
  behaviourAccn.YValue = 1;

  return behaviourAccn;
}

Vector2D Behaviours::Flee() // NOT COMPLETED
{
  Vector2D behaviourAccn;
  behaviourAccn.XValue = 1;
  behaviourAccn.YValue = 1;

  return behaviourAccn;
}

Vector2D Behaviours::WallAvoid(Vector2D botPosition, Vector2D botVelocity)
{
  Circle2D colCircle(botPosition, 70);
  Vector2D acc = StaticMap::GetInstance()->GetNormalToSurface(colCircle);

  if (botVelocity == Vector2D(0, 0))
    acc * 100;
  else
    acc * 60;

  return acc;
}

void Behaviours::Release()
{
  if (pInstance)
  {
    delete pInstance;
    pInstance = nullptr;
  }
}
