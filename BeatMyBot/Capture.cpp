#include "Capture.h"
#include "Pathfinder.h"
#include "dynamicObjects.h"
#include "staticmap.h"

Capture* Capture::pInstance = nullptr;

Capture::Capture()
{

} // Capture()


Capture* Capture::GetInstance()
{

  if (!pInstance)
    pInstance = new Capture;

  return pInstance;

} // GetInstance()


void Capture::Enter(Bot* pBot)
{
  pBot->pTarget = nullptr;
  pBot->SetBehaviours(0, 0, 0, 0, 0, 1, 1);

} // Enter()


void Capture::Execute(Bot* pBot)
{

  if (pBot->IsAlive() && pBot->GetPath()->size() == 0 && DynamicObjects::GetInstance()->m_iNumPlacedDominationPoints > 0)
  {
    pBot->domTarget = &DynamicObjects::GetInstance()->GetDominationPoint(0);
    GetPath(pBot);
  }

  if (pBot->IsAlive() && DynamicObjects::GetInstance()->m_iNumPlacedDominationPoints > 0)
  {
    if (StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(),
      DynamicObjects::GetInstance()->GetDominationPoint(0).m_Location))
    {
      // Turn on seek and wall avoid
      pBot->SetBehaviours(1, 0, 0, 0, 0, 1, 0);
    }


    pBot->SetAcceleration(pBot->Accumulate(DynamicObjects::GetInstance()->GetDominationPoint(0).m_Location, Vector2D(0, 0),
      pBot->GetLocation(), pBot->GetVelocity(), pBot->GetPath()));
  }
} // Execute()

void Capture::Exit(Bot* pBot)
{

} // Exit()


void Capture::GetPath(Bot* pBot)
{

  pBot->SetPath(&Pathfinder::GetInstance()->GeneratePath(pBot->GetLocation(),
    pBot->domTarget->m_Location));
  

} // GetPath()


void Capture::SetTarget(Bot* pBot, DominationPoint* target)
{

  pBot->domTarget = target;

} // SetTarget()


void Capture::Release()
{
  if (pInstance)
  {
    delete pInstance;
    pInstance = nullptr;
  }

} // Release()


Capture::~Capture()
{

} // ~Capture()