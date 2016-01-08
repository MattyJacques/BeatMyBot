// Title        : Attack.cpp
// Purpose      : Allows AI to attack an enemy bot
//

#include "Attack.h"             
#include "staticmap.h"            // For LineOfSight()
#include "dynamicObjects.h"       // For GetBot() MAYBE NOT NEEDED
#include "Reload.h"
#include "Capture.h"


// Initialise pInstance as nullptr
Attack* Attack::pInstance = nullptr;


Attack::Attack()
{
  sprintf_s(name, "Attacking");
} // Attack()


Attack* Attack::GetInstance()
{ // Returns the instance of the class, if none currently exists create one

  if (!pInstance)
    pInstance = new Attack;

  return pInstance;

} // GetInstance()


void Attack::Enter(Bot* pBot)
{ // Sets behaviours for the attack state

  // Turn on Pursue and avoid walls
  pBot->SetBehaviours(0, 0, 1, 0, 0, 1, 0);

} // Enter()


void Attack::Execute(Bot* pBot)
{ // Checks for line of sight to enemy bot, if so switches behaviours. If close
  // enough, aims at bot and fires is accuracy check passes

  if (StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(), 
    DynamicObjects::GetInstance()->GetBot(1, pBot->botTarget).GetLocation()))
  { // If LineOfSight, adjust behaviours and do distance check

    // Turn on arrive and avoid walls
    pBot->SetBehaviours(0, 1, 0, 0, 0, 1, 0);

    if ((pBot->GetLocation() - DynamicObjects::GetInstance()->GetBot(1, pBot->botTarget).GetLocation()).magnitude() <= 300)
    { // Check if distance is below set amount, if so start aiming and check
      // accuracy

      if (pBot->GetTargetTeam() != pBot->GetTeamNumber() + 1)
      { // Tells the bot to start aiming at target if not already

        pBot->SetTarget(pBot->GetTeamNumber() + 1 == 1, pBot->botTarget);
      }

      // If accuracy is above set amount, shoot at enemy
      if (pBot->GetAccuracy() >= 0.5)
        pBot->Shoot();

    } // if magnitude()

  } // if LineOfSight()

  if (!DynamicObjects::GetInstance()->GetBot(pBot->GetTeamNumber() + 1 == 1, pBot->botTarget).IsAlive() || !pBot->IsAlive() || StaticMap::GetInstance()->IsLineOfSight(DynamicObjects::GetInstance()->GetBot(pBot->GetTeamNumber() + 1 == 1, pBot->botTarget).GetLocation(), pBot->GetLocation()))
  {
    pBot->botTarget = -1;
    pBot->ChangeState(Capture::GetInstance());
  }

  if (pBot->GetAmmo() == 0)
    pBot->ChangeState(Reload::GetInstance());


} // Execute()


void Attack::Exit(Bot* pBot)
{ // Tidies up after the state activity is completed
  pBot->botTarget = -1;
  pBot->domTarget = -1;
} // Exit()


char* Attack::GetStateName()
{ // Returns the name of the state

  return name;

} // GetStateName()


void Attack::Release()
{ // If called while pInstance is valid, deletes and defines as nullptr

  if (pInstance)
  {
    delete pInstance;
    pInstance = nullptr;
  }

} // Release()