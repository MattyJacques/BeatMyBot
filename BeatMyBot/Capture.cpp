// Title        : Capture.h
// Purpose      : Handles the bot being sent on a magical journey to capture DP


#include "Capture.h"
#include "Pathfinder.h"           // Able to generate a path    
#include "dynamicObjects.h"       // DP access
#include "staticmap.h"            // LineOfSight()


// Initialise pInstance as nullptr
Capture* Capture::pInstance = nullptr;


Capture* Capture::GetInstance()
{ // Returns the instance of the class, if none currently exists create one

  if (!pInstance)
    pInstance = new Capture;

  return pInstance;

} // GetInstance()


void Capture::Enter(Bot* pBot)
{ // Sets behaviours for the capture state

  pBot->pTarget = nullptr;
  pBot->SetBehaviours(0, 0, 0, 0, 0, 1, 1);

} // Enter()


void Capture::Execute(Bot* pBot)
{ // Checks to see if there is a lineofsight to a DP, if so switches behaviours
  // otherwise just continue following path

  // EDIT THIS FOR CUSTOM CAPTURE
  if (pBot->IsAlive() && pBot->GetPath()->size() == 0 && 
    DynamicObjects::GetInstance()->m_iNumPlacedDominationPoints > 0)
  { // If bot is alive and we have no path and DPs are placed, get a path

    pBot->domTarget = &DynamicObjects::GetInstance()->GetDominationPoint(0);
    GetPath(pBot);

  }

  if (pBot->IsAlive() && DynamicObjects::GetInstance()->m_iNumPlacedDominationPoints > 0)
  { // If alive and DPs are placed, check for line of sight

    if (StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(),
      DynamicObjects::GetInstance()->GetDominationPoint(0).m_Location))
    { // If we can see the DP, turn on seek and wall avoid
      
      pBot->SetBehaviours(1, 0, 0, 0, 0, 1, 0);

    }

    pBot->SetAcceleration(pBot->Accumulate(DynamicObjects::GetInstance()->GetDominationPoint(0).m_Location, Vector2D(0, 0),
      pBot->GetLocation(), pBot->GetVelocity(), pBot->GetPath()));

  } // If alive and DPs are placed

} // Execute()


void Capture::Exit(Bot* pBot)
{ // Tidies up after the state activity has completed

} // Exit()


void Capture::GetPath(Bot* pBot)
{ // Calls GeneratePath to get a path to desired DP, then sets the bots path
  // to the generated path

  pBot->SetPath(&Pathfinder::GetInstance()->GeneratePath(pBot->GetLocation(),
    pBot->domTarget->m_Location));

} // GetPath()


void Capture::SetTarget(Bot* pBot, DominationPoint* target)
{ // Sets the DP target of the bot with the given parameter

  pBot->domTarget = target;

} // SetTarget()


void Capture::Release()
{ // If pInstance is valid, deletes the instance then defines al nullptr

  if (pInstance)
  {
    delete pInstance;
    pInstance = nullptr;
  }

} // Release()