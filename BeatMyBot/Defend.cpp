// Title        : Defend.cpp
// Purpose      : Allows AI to defend a DP

#include "Defend.h"
#include "dynamicObjects.h"
#include "Pathfinder.h"
#include "staticmap.h"
#include "Attack.h"
#include "Capture.h"

// Initalise to null to avoid junk
Defend* Defend::pInstance = nullptr;

Defend::Defend()
{ // Private constructor to avoid unauth access
  sprintf_s(name, "Defending");
} // Defend()


Defend* Defend::GetInstance()
{ // Returns the instance of the class, if none currently exists create one

  if (!pInstance)
  {
    pInstance = new Defend;
  }

  return pInstance;

} // GetInstance()


void Defend::Enter(Bot* pBot)
{ // Sets behaviours for defend state

  pBot->domTarget = -1;
  pBot->botTarget = -1;
  pBot->SetBehaviours(0, 0, 0, 0, 0, 1, 1); // Wall avoid and follow path

} // Enter()


void Defend::Execute(Bot* pBot)
{ // Checks if we have a target to defend, if not chooses the closest DP. 
  // Travels to DP then waits for enemy to get close to DP and switches to
  // attack.


  if (pBot->IsAlive() && DynamicObjects::GetInstance()->m_iNumPlacedDominationPoints > 0)
  {
    // If we don't have a target, create one
    if (pBot->domTarget == -1)
      SetTarget(pBot, -1);

    SetBehaviours(pBot); // Check how close we are to DP

    pBot->SetAcceleration(pBot->Accumulate(DynamicObjects::GetInstance()->GetDominationPoint(pBot->domTarget).m_Location,
      Vector2D(0, 0), pBot->GetLocation(), pBot->GetVelocity(), pBot->GetPath()));

    IsEnemyClose(pBot);  // Check if any enemys are close to the DP

    //// If no enemies are close to DP do we have LOS on any bots
    //if (pBot->botTarget == -1 && (pBot->GetLocation() -
    //  DynamicObjects::GetInstance()->GetDominationPoint(pBot->domTarget).m_Location).magnitude()
    //  < DOMINATIONRANGE * 2)
    //{
    //  CheckLineOfSight(pBot);
    //}

    AimAttack(pBot);  // Aim at enemy and change state to attack

  } // If alive and DPs are placed
  else
  {
    pBot->domTarget = -1; // Else if we are dead, reset target
    pBot->ChangeState(Capture::GetInstance());
  }
} // Execute()


void Defend::Exit(Bot* pBot)
{
  pBot->botTarget = -1;
  pBot->domTarget = -1;
} // Exit()


void Defend::SetTarget(Bot* pBot, int target)
{ // Set bots DP target to given value, if value is null find closest DP

  if (target == -1)
    pBot->domTarget = GetClosestDP(pBot);
  else
    pBot->domTarget = target;

  if (!StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(), 
    DynamicObjects::GetInstance()->GetDominationPoint(target).m_Location))
  {
    pBot->SetPath(&Pathfinder::GetInstance()->GeneratePath(pBot->GetLocation(),
     DynamicObjects::GetInstance()->GetDominationPoint(target).m_Location));
  }

} // SetTarget()


int Defend::GetClosestDP(Bot* pBot)
{ // Returns the closest DP to the bots location

  int closestDP = -1;   // Holds the closest DP
  float closeDis = FLT_MAX; // Holds closest DP distance

  for (int i = 0; i < NUMDOMINATIONPOINTS; i++)
  { // Check all DPs for the closest

    if (DynamicObjects::GetInstance()->GetDominationPoint(i).m_OwnerTeamNumber
      == 0 && (pBot->GetLocation() -
      DynamicObjects::GetInstance()->GetDominationPoint(i).m_Location).magnitude()
      < closeDis)
    { // Store data on closest DP if new closest has been found
      closeDis = DynamicObjects::GetInstance()->GetDominationPoint(i).m_Location.magnitude();
      closestDP = i;
    }

  }

  return closestDP;

} // GetClosestDP()


void Defend::IsEnemyClose(Bot* pBot)
{ // Checks all enemy bots to see if any are close to the DP we are defending
  // and stores the closest bot to the DP

  int closestBot = -1;
  float closestDist = FLT_MAX;

  for (int i = 0; i < NUMBOTSPERTEAM; i++)
  { // Loop through all enemy bots to test if they are close to the DP

    // THIS LOOKS NASTY
    // Checks to see if the enemy is close and is the closest found so far.
    // If so, store pointer
    if ((DynamicObjects::GetInstance()->GetBot(1, i).GetLocation() -
      DynamicObjects::GetInstance()->GetDominationPoint(pBot->domTarget).m_Location).magnitude() < 500 &&
      (DynamicObjects::GetInstance()->GetBot(1, i).GetLocation() -
      DynamicObjects::GetInstance()->GetDominationPoint(pBot->domTarget).m_Location).magnitude() < closestDist 
      && DynamicObjects::GetInstance()->GetBot(1, i).IsAlive())
    {
      closestBot = i;
    }
  }

  pBot->botTarget = closestBot;     // Save the target for attack

} // IsEnemyClose()


void Defend::SetBehaviours(Bot* pBot)
{ // Checks how close we are to the DP, if we are close, arrive else seek

  if (StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(),
    DynamicObjects::GetInstance()->GetDominationPoint(pBot->domTarget).m_Location))
  { // If we can see the DP, switch behaviours

    if ((pBot->GetLocation() - DynamicObjects::GetInstance()->GetDominationPoint(pBot->domTarget).m_Location).magnitude() < DOMINATIONRANGE * 5)
    { // If we are close to the DP, stand on top of it so enemy can't capture while we are alive
      pBot->SetBehaviours(0, 1, 0, 0, 0, 1, 0);
    }
    else
    {
      pBot->SetBehaviours(1, 0, 0, 0, 0, 1, 0); // Wall avoid and seek
    }
  }

} // SetBehaviours()


void Defend::AimAttack(Bot* pBot)
{ // Aims at the enemy bot and switches to attack state

  if (pBot->botTarget != -1)
  { // If we have a target, switch to the attack state
    pBot->SetTarget(1, pBot->botTarget);
    pBot->ChangeState(Attack::GetInstance());
  }

} // AimAttack()


void Defend::CheckLineOfSight(Bot* pBot)
{
  for (int i = 0; i < MAXBOTSPERTEAM; i++)
  {
    if (StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(), DynamicObjects::GetInstance()->GetBot(1, i).GetLocation()))
    {
      pBot->botTarget = i;
    }
  }
} // CheckLineOfSight()


char* Defend::GetStateName()
{ // Returns the name of the state

  return name;

} // GetStateName()


void Defend::Release()
{ // If called while pInstance is valid, deletes and defines as nullptr

  if (pInstance)
  {
    delete pInstance;
    pInstance = nullptr;
  }

} // Release()