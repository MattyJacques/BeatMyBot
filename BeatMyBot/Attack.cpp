#include "Attack.h"
#include "staticmap.h"


Attack* Attack::pInstance = nullptr;


Attack::Attack()
{

} // Attack()


Attack* Attack::GetInstance()
{
  if (!pInstance)
    pInstance = new Attack;

  return pInstance;

} // GetInstance()


void Attack::Enter(Bot* pBot, Bot* targetBot)
{
  target = targetBot;

  // Turn on Pursue and avoid walls
  pBot->SetBehaviours(0, 0, 1, 0, 0, 1, 0);
} // Enter()


void Attack::Execute(Bot* pBot)
{
  if (StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(), target->GetLocation()))
  {
    // Turn on arrive and avoid walls
    pBot->SetBehaviours(0, 1, 0, 0, 0, 1, 0);
    if ((pBot->GetLocation() - target->GetLocation()).magnitude() <= 150)
    {
      pBot->SetTarget(target->GetTeamNumber(), target->GetBotNumber());
      if (pBot->GetAccuracy() >= 0.1)
        pBot->Shoot();
    }
  }

  pBot->Accumulate(target->GetLocation(), target->GetVelocity(), 
                    pBot->GetLocation(), pBot->GetVelocity(), pBot->GetPath());

} // Execute()


void Attack::Exit(Bot* pBot)
{

} // Exit()