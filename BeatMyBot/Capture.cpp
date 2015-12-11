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
  pBot->domTarget = &DynamicObjects::GetInstance()->GetDominationPoint(0);
  pBot->SetBehaviours(0, 0, 0, 0, 0, 1, 1);

} // Enter()


void Capture::Execute(Bot* pBot)
{
  if (pBot->IsAlive() && pBot->GetPath().empty() && DynamicObjects::GetInstance()->m_iNumPlacedDominationPoints > 0)
  {
    GetPath(pBot);
  }

  if (StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(),
                                               pBot->domTarget->m_Location))
  {
    // Turn on seek and wall avoid
    pBot->SetBehaviours(1,0,0,0,0,1,0);
  }

  pBot->SetAcceleration(pBot->Accumulate(pBot->domTarget->m_Location, Vector2D(0, 0),
    pBot->GetLocation(), pBot->GetVelocity(), pBot->GetPath()));
} // Execute()

void Capture::Exit(Bot* pBot)
{

} // Exit()


void Capture::GetPath(Bot* pBot)
{

  pBot->SetPath(&Pathfinder::GetInstance()->GeneratePath(pBot->GetLocation(),
    pBot->domTarget->m_Location));

} // GetPath()


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