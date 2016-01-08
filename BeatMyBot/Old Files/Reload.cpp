// Title        : Reload.cpp
// Purpose      : Handles the bot being able to find a supply point for reload

#include "Reload.h"
#include "staticmap.h"
#include "Pathfinder.h"
#include "Capture.h"


Reload* Reload::pInstance = nullptr;


Reload::Reload()
{
  sprintf_s(name, "Reloading");
} // Reload()



Reload* Reload::GetInstance()
{ // Returns the instance of the class, if none currently exists create one

  if (!pInstance)
    pInstance = new Reload;

  return pInstance;

} // GetInstance()


void Reload::Enter(Bot* pBot)
{ // Sets behaviours for the capture state

  pBot->domTarget = -1;
  pBot->botTarget = -1;
  pBot->myPath.empty();
  pBot->SetBehaviours(0, 0, 0, 0, 0, 1, 1);

} // Enter()


void Reload::Execute(Bot* pBot)
{ // Checks to see if there is a lineofsight to a reload point, if so switches 
  // behaviours otherwise just continue following path

  if (pBot->IsAlive() && pBot->GetAmmo() <= 0)
  {
    // If we don't have a target, create one
    if (pBot->domTarget == -1)
      SetTarget(pBot);

    SetBehaviours(pBot);

    pBot->SetAcceleration(pBot->Accumulate(StaticMap::GetInstance()->GetResupplyLocation(pBot->domTarget),
      Vector2D(0, 0), pBot->GetLocation(), pBot->GetVelocity(), pBot->GetPath()));

  }
  else
  {
    pBot->ChangeState(Capture::GetInstance());
  }

} // Execute()


void Reload::Exit(Bot* pBot)
{ // Tidies up after the state activity has completed

} // Exit()


int Reload::GetClosestSupply(Vector2D pos)
{ // Calculates the closest point a bot can reload from

  int closestID = -1;   // Holds the closest RP
  float closeDis = FLT_MAX; // Holds closest rP distance

  for (int i = 0; i < StaticMap::GetInstance()->GetNumSupplyPoints(); i++)
  { // Check all RPs for the closest

    if ((StaticMap::GetInstance()->GetResupplyLocation(i) - pos).magnitude() 
      < closeDis)
    {
      closestID = i;
      closeDis = (StaticMap::GetInstance()->GetResupplyLocation(i) - pos).magnitude();
    }
  }

  return closestID;

} // GetClosestSupply()


void Reload::SetTarget(Bot* pBot)
{ // Sets the target and creates the path

  pBot->domTarget = GetClosestSupply(pBot->GetLocation());

  if (!StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(), 
    StaticMap::GetInstance()->GetResupplyLocation(pBot->domTarget)))
  {
    pBot->SetPath(&Pathfinder::GetInstance()->GeneratePath(pBot->GetLocation(), 
      StaticMap::GetInstance()->GetResupplyLocation(pBot->domTarget)));
  }

} // SetTarget()


void Reload::SetBehaviours(Bot* pBot)
{ // Checks how close we are to the RP, if we are close, arrive else seek

  if (StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(),
    StaticMap::GetInstance()->GetResupplyLocation(pBot->domTarget)))
  { // If we can see the RP, switch behaviours

    if ((pBot->GetLocation() - StaticMap::GetInstance()->GetResupplyLocation(pBot->domTarget)).magnitude() < DOMINATIONRANGE * 5)
    { // If we are close to the DP, stand on top of it so enemy can't capture while we are alive
      pBot->SetBehaviours(0, 1, 0, 0, 0, 1, 0);
    }
    else
    {
      pBot->SetBehaviours(1, 0, 0, 0, 0, 1, 0); // Wall avoid and seek
    }
  }

} // SetBehaviours()


char* Reload::GetStateName()
{ // Returns the name of the state

  return name;

} // GetStateName()


void Reload::Release()
{ // If pInstance is valid, deletes the instance then defines al nullptr

  if (pInstance)
  {
    delete pInstance;
    pInstance = nullptr;
  }

} // Release()
