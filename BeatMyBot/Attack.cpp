#include "Attack.h"
#include "staticmap.h"
#include "dynamicObjects.h"


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


void Attack::Enter(Bot* pBot)
{
  GetTarget(pBot);

  // Turn on Pursue and avoid walls
  pBot->SetBehaviours(0, 0, 1, 0, 0, 1, 0);
} // Enter()


void Attack::Execute(Bot* pBot)
{
  if (StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(), 
    pBot->pTarget->GetLocation()))
  {
    // Turn on arrive and avoid walls
    pBot->SetBehaviours(0, 1, 0, 0, 0, 1, 0);
    if ((pBot->GetLocation() - pBot->pTarget->GetLocation()).magnitude() <= 300)
    {
      pBot->SetTarget(pBot->pTarget->GetTeamNumber(), 
        pBot->pTarget->GetBotNumber());

      if (pBot->GetAccuracy() >= 0.8)
        pBot->Shoot();
    }
  }

} // Execute()


void Attack::GetTarget(Bot* pBot)
{
  int targetTeam;

  if (pBot->GetTeamNumber() == 0)
    targetTeam = 1;
  else
    targetTeam = 0;


  pBot->pTarget = &DynamicObjects::GetInstance()->GetBot(targetTeam, 
                                                         pBot->GetBotNumber());
} // GetTarget()


void Attack::Exit(Bot* pBot)
{

} // Exit()


void Attack::Release()
{
  if (pInstance)
  {
    delete pInstance;
    pInstance = nullptr;
  }
} // Release()


Attack::~Attack()
{

} // ~Attack()
