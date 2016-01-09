// Title: Attack.h
// Purpose: Allows to bot to find a target and shoot


#include "Attack.h"
#include "staticmap.h"
#include "dynamicObjects.h"
#include "Reload.h"
#include "Capture.h"


Attack* Attack::pInst = nullptr;


Attack::Attack()
{ // Private constructor for singleton, sets name of the class for debugging

  sprintf_s(name, "Attacking");

} // Attack()


Attack* Attack::GetInstance()
{ // Returns the current instance of the class, if one does not currently
  // exist, creates on then returns pInst

  if (!pInst)
    pInst = new Attack;

  return pInst;

} // GetInstance()


void Attack::Enter(Bot* pBot)
{ // Calculates the target that the bot is going to be attacking and sets
  // behaviours to pursue and avoid walls

  GetTarget(pBot);
  pBot->SetBehaviours(0, 0, 1, 0, 0, 1, 0);

} // Enter()


void Attack::Execute(Bot* pBot)
{ // Checks for the line of the sight between the bot and the enemy, checks
  // ammo and checks for enemies health to see if the bot can leave the
  // attacking state


  if (pBot->botTarget != -1)
  {
    if (pBot->GetAmmo() < 1)                  // If bot has no ammo
      pBot->ChangeState(Reload::GetInstance());
    else if (!TargetAlive(pBot) || !HaveLOS(pBot)) // If no target anymore
      pBot->ChangeState(Capture::GetInstance());
    else if (DistanceToEnemy(pBot) > 400)       // If too far from target
      pBot->SetAcceleration(pBot->Accumulate(DynamicObjects::GetInstance()->
        GetBot(1, pBot->botTarget).GetLocation(), DynamicObjects::GetInstance()->
        GetBot(1, pBot->botTarget).GetVelocity(), pBot->GetLocation(),
        pBot->GetVelocity(), pBot->GetPath()));
    else if (pBot->GetTargetTeam() != 1)        // If not aiming
      pBot->SetTarget(1, pBot->botTarget);

    if (pBot->GetAccuracy() > 0.7)
      pBot->Shoot();
  }
  else
  {
    pBot->ChangeState(Capture::GetInstance());
  }

} // Execute()


void Attack::Exit(Bot* pBot)
{ // Tidies up after the states activities, getting ready for the next state

  pBot->StopAiming();
  pBot->botTarget = -1;

} // Exit()


void Attack::GetTarget(Bot* pBot)
{ // Searches for an enemy bot within range and in line of sight, sets the bots
  // target to the ID of the bot found and starts aiming

  for (int i = 0; i < NUMBOTSPERTEAM; i++)
  {
    // If the bot has a line of the sight to another bot, plus if the distance
    // is below 400, marks as there is a enemy close and starts aiming at the
    // bot found
    if (StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(),
      DynamicObjects::GetInstance()->GetBot(1, i).GetLocation()) &&
      (pBot->GetLocation() - DynamicObjects::GetInstance()->GetBot(1, i).
      GetLocation()).magnitude() <= 400 && DynamicObjects::GetInstance()->
      GetBot(1, i).IsAlive())
    {
      pBot->botTarget = i;
      pBot->SetTarget(1, i);
    }
  }

} // GetTarget()


bool Attack::HaveLOS(Bot* pBot)
{ // Checks if there is a line of sight between the bot and the target bot

  return StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(), 
    DynamicObjects::GetInstance()->GetBot(1, pBot->botTarget).GetLocation());

} // HaveLOS()


bool Attack::TargetAlive(Bot* pBot)
{ // Checks to see if the target is still alive

  return DynamicObjects::GetInstance()->GetBot(1, pBot->botTarget).IsAlive();

} // TargetAlive()


float Attack::DistanceToEnemy(Bot* pBot)
{ // Returns the distance inbetween the bot and an enemy

  return (pBot->GetLocation() - DynamicObjects::GetInstance()->GetBot(1, pBot->botTarget).GetLocation()).magnitude();

} // DistanceToEnemy()


char* Attack::GetStateName()
{ // Returns the name of the state for debugging purposes

  return name;

} // GetStateName()


void Attack::Release()
{ // If called when pInst is valid, deletes the instance and defines as nullptr

  if (pInst)
  {
    delete pInst;
    pInst = nullptr;
  }

} // Release()