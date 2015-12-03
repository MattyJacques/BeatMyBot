#pragma once

#include "vector2D.h"
#include "bot.h"

class Behaviours
{
protected:
  Behaviours();

public:
  bool seekOn;
  bool pathfindOn;
  bool avoidWallsOn;
  static Behaviours* pInstance;

  static Behaviours* GetInstance();
  Vector2D Seek(Vector2D targetPosition, Vector2D botPosition,
                Vector2D botVelocity);
  Vector2D Arrive(Vector2D targetPosition, Vector2D botPosition,
                  Vector2D botVelocity);
  Vector2D Pursue(Vector2D targetPosition, Vector2D targetVelocity,
                  Vector2D botPosition, Vector2D botVelocity);
  Vector2D Evade();
  Vector2D Flee();
  Vector2D WallAvoid(Vector2D botPosition, Vector2D botVelocity);
  static void Release();
};

