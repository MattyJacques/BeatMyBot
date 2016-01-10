// Title: Reload.h
// Purpose: Allows to bot to venture and resupply at one of the points


#include "Reload.h"
#include "staticmap.h"
#include "Pathfinder.h"
#include "Capture.h"


Reload* Reload::pInst = nullptr;


Reload::Reload()
{ // Private constructor for singleton, sets the name of the class

  sprintf_s(name, "Reloading");

} // Reload()


Reload* Reload::GetInstance()
{ // Returns the instance of the class, if no current instance, creates one
  // and returns pInst

  if (!pInst)
    pInst = new Reload;

  return pInst;

} // GetInstance()


void Reload::Enter(Bot* pBot)
{ // Calculates the closest supply point and gets a path to that supply
  // point. Turns on wall avoid and followPath

  SetTarget(pBot);
  pBot->SetBehaviours(0, 0, 0, 0, 0, 1, 1);

} // Enter


void Reload::Execute(Bot* pBot)
{ // Checks the distance to the supply point while checking for line of sight
  // to change behaviours, checks ammo to make sure we haven't already 
  // resupplied and sets the acceleration for the bot

  if (pBot->GetAmmo() == MAXAMMO)
    pBot->ChangeState(Capture::GetInstance());


  SetBehaviours(pBot);

  pBot->SetAcceleration(pBot->Accumulate(pBot->supplyLoc, Vector2D(0, 0),
    pBot->GetLocation(), pBot->GetVelocity(), pBot->GetPath()));

} // Execute()


void Reload::Exit(Bot* pBot)
{ // Tidies up after the activities of the state

} // Exit()


void Reload::SetTarget(Bot* pBot)
{ // Gets the closest resupply point and gets the path to that supply point

  GetClosestResupply(pBot);
  //pBot->supplyLoc = StaticMap::GetInstance()->GetClosestResupplyLocation(pBot->GetLocation());
  pBot->SetPath(&Pathfinder::GetInstance()->GeneratePath(pBot->GetLocation(), pBot->supplyLoc));

} // SetTarget()


void Reload::SetBehaviours(Bot* pBot)
{ // Checks to see if the bots behaviours needs to switch to arrive or seek

  if (StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(), pBot->supplyLoc))
  {
    if ((pBot->GetLocation() - pBot->supplyLoc).magnitude() < 100)
      pBot->SetBehaviours(0, 1, 0, 0, 0, 1, 0);
    else
      pBot->SetBehaviours(1, 0, 0, 0, 0, 1, 0);

    pBot->GetPath()->clear();
  }

} // SetBehaviours()


void Reload::GetClosestResupply(Bot* pBot)
{ // Sets the bots target as the vector of the cloest resupply location on
  // the bots teams side of the map

  if ((StaticMap::GetInstance()->GetResupplyLocation(2) - pBot->GetLocation()).magnitude() > (StaticMap::GetInstance()->GetResupplyLocation(3) - pBot->GetLocation()).magnitude())
    pBot->supplyLoc = StaticMap::GetInstance()->GetResupplyLocation(3);
  else
    pBot->supplyLoc = StaticMap::GetInstance()->GetResupplyLocation(2);

} // GetClosestResupply()


char* Reload::GetStateName()
{ // Returns the state name from debugging

  return name;

} // GetStateName()


void Reload::Release()
{ // If pInst is valid, deletes and sets to nullptr
  
  if (pInst)
  {
    delete pInst;
    pInst = nullptr;
  }

} // Release()