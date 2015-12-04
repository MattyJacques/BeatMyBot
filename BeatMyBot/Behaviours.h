#pragma once

#include "vector2D.h"       // Vector2D functionality

class Behaviours
{
private:

 

public:
  bool seekOn;          // Is the bot seeking
  bool pathfindOn;      // Is the bot pathfinding
  bool avoidWallsOn;    // Is the bot avoiding walls
  
  // Constructor initialises all variables within class to null etc
  Behaviours();

  // Takes in target position and returns desired velocity to reach the target
  Vector2D Seek(Vector2D targetPosition, Vector2D botPosition,
                Vector2D botVelocity);

  // Returns a vector2D that can be used to arrive at a target position by 
  // slowing down the velocity of the bot 
  Vector2D Arrive(Vector2D targetPosition, Vector2D botPosition,
                  Vector2D botVelocity);

  // Returns a vector2D that can be used to pursue a moving target using the
  // targets position and velocity to works out an intercept
  Vector2D Pursue(Vector2D targetPosition, Vector2D targetVelocity,
                  Vector2D botPosition, Vector2D botVelocity);

  // Takes in the target the bot is running from and provides a vector2D for
  // use in the Flee()
  Vector2D Evade();

  // Takes in a vector2D and returns a velocity directly away from that point
  Vector2D Flee();

  // Uses a circle2D as a collision box provided a velocity that can be used
  // the avoid nearby walls
  Vector2D WallAvoid(Vector2D botPosition, Vector2D botVelocity);

  // Accumulates all the vector2D velocitys from the other behaviours to
  // provide the real velcoity the bot should currently be following
  Vector2D Accumulate(Vector2D targetPosition, Vector2D targetVelocity,
                      Vector2D botPosition, Vector2D botVelocity);

};

