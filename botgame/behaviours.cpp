#include "behaviours.h"

Vector2D Behaviours::Seek()
{
	Vector2D desiredVelocity = (targetPosition - botPosition).unitVector() * MAXBOTSPEED;

	Vector2D behaviourAccn = (desiredVelocity-botVelocity).unitVector() * MAXIMUMACCELERATION;
	return behaviourAccn;
}

Vector2D Behaviours::Flee()
{
	return -Seek();
}

Vector2D Behaviours::Pursue()
{
	double distance = (targetPosition - botPosition).magnitude();

	double time = distance/MAXBOTSPEED;

	Vector2D target = targetPosition + targetVelocity*time;

	targetPosition = target;

	return Seek();
}

Vector2D Behaviours::Arrive()
{
	double speed = (targetPosition - botPosition).magnitude() / 5;

	if (speed > MAXBOTSPEED)
		speed = MAXBOTSPEED;

	Vector2D desiredVelocity = (targetPosition - botPosition).unitVector() * speed;

	Vector2D behaviourAccn = desiredVelocity-botVelocity;
	return behaviourAccn;
}

Vector2D Behaviours::WallAvoid()
{
	Circle2D wallCircle(botPosition, 70);
	Vector2D accel; 
	accel = StaticMap::GetInstance()->GetNormalToSurface(wallCircle);
	if(botVelocity == Vector2D(0,0))
		return accel * 100;
	return (accel * 60);
}

Vector2D Behaviours::FollowPath()
{
	if(!thePath.empty())
	{
		targetPosition = thePath.back();
		if(thePath.size() > 1)
		{
			if(StaticMap::GetInstance()->IsLineOfSight(botPosition, thePath[thePath.size() - 2]))
				targetPosition = thePath[thePath.size() - 2];

			Circle2D checkCircle(botPosition, 30);
			if(checkCircle.Intersects(Point2D(targetPosition)))
			{
				thePath.pop_back();
				targetPosition = thePath.back();
			}
		}
	}
	return Seek();
}

Vector2D Behaviours::ReturnAcceleration()
{
	Vector2D acceleration;
	if(seekOn)
		acceleration += Seek();
	if(fleeOn)
		acceleration += Flee();
	if(pursueOn)
		acceleration += Pursue();
	if(arriveOn)
		acceleration += Arrive();
	if(wallAvoidOn)
		acceleration += WallAvoid();
	if(pathFollowOn)
		acceleration += FollowPath();

	return acceleration;
}

void Behaviours::FindPath()
{
	thePath = Pathfinder::GetInstance()->GeneratePath(botPosition, targetPosition);
}

void Behaviours::SetUp(Vector2D position, Vector2D velocity, Vector2D target, Vector2D targetsVelocity)
{
	botPosition.set(position);
	botVelocity.set(velocity);
	targetPosition.set(target);
	targetVelocity.set(targetsVelocity);
}

void Behaviours::RefreshBot(Vector2D position, Vector2D velocity)
{
	botPosition.set(position);
	botVelocity.set(velocity);
}

void Behaviours::ConfigureBehaviours(bool seek, bool flee, bool pursue, bool arrive, bool wavoid, bool pathfollow)
{
	seekOn = seek;
	fleeOn = flee;
	pursueOn = pursue;
	arriveOn = arrive;
	wallAvoidOn = wavoid;
	pathFollowOn = pathfollow;
}