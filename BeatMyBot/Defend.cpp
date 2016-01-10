// Title: Defend.cpp
// Purpose: Controls the bot while defending a domination point


#include "Defend.h"
#include "Pathfinder.h"
#include "dynamicObjects.h"
#include "staticmap.h"
#include "Attack.h"
#include "Capture.h"


Defend* Defend::pInst = nullptr;


Defend::Defend()
{ // Private constructor for singleton, sets name of state for debugging

  sprintf_s(name, "Defending");

} // Defend()


Defend* Defend::GetInstance()
{ // Returns the current instance of the class, if none currently exists,
  // create one and return pInst

  if (!pInst)
    pInst = new Defend;

  return pInst;

} // GetInstance()


void Defend::Enter(Bot* pBot)
{ // Sets the domination point the bot is defending then proceeds to ask for
  // the path to that domination point setting the path in bot. Sets the 
  // behaviours to wall avoid and follow path

  SetTarget(pBot);
  pBot->SetBehaviours(0, 0, 0, 0, 0, 1, 1);

} // Enter()


void Defend::Execute(Bot* pBot)
{ // Checks to see if we are close to the domination point the bot is defending
  // Checks to see if there are any enemy bots within range to attack then makes
  // sure the bots team still owns the domination point

  CheckDistance(pBot);
  IsEnemyClose(pBot);

  if (!StillOwnDP(pBot))
    pBot->ChangeState(Capture::GetInstance());

  pBot->SetAcceleration(pBot->Accumulate(DynamicObjects::GetInstance()->
    GetDominationPoint(pBot->domTarget).m_Location, Vector2D(0, 0),
    pBot->GetLocation(), pBot->GetVelocity(), pBot->GetPath()));

} // Execute()


void Defend::Exit(Bot* pBot)
{ // Tidies up after the activities of the state

} // Exit()


void Defend::SetTarget(Bot* pBot)
{ // Sets the target the bots needs to defend, calls to generate the path to
  // the target

  // Set the target
  pBot->domTarget = pBot->GetBotNumber() % 3;

  // Generate path to domination point
  pBot->SetPath(&Pathfinder::GetInstance()->GeneratePath(pBot->GetLocation(),
    DynamicObjects::GetInstance()->GetDominationPoint(pBot->domTarget).m_Location));

} // SetTarget()


void Defend::CheckDistance(Bot* pBot)
{ // Checks the distance from the bot to the domination point, if the bot has
  // line of sight to the domination point, switch to seek, if close to the 
  // domination point, switch to arrive 

  // If the bot can see the domination point, check to see if the bot is close.
  // If not close, then switch to seek
  if (StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(),
    DynamicObjects::GetInstance()->GetDominationPoint(pBot->domTarget).m_Location))
  {
    if ((pBot->GetLocation() - DynamicObjects::GetInstance()->
      GetDominationPoint(pBot->domTarget).m_Location).magnitude() < 100)
    {
      pBot->SetBehaviours(0, 1, 0, 0, 0, 1, 0);    
    }
    else
    {
      pBot->SetBehaviours(1, 0, 0, 0, 0, 1, 0);
      pBot->GetPath()->clear();
    }
  }

} // CheckDistance()


void Defend::IsEnemyClose(Bot* pBot)
{ // Checks to see if there are enemy bots in range of the domination point the
  // bot is defending, if so switch to the attack state

  // Loop through all enemy bots on the next team
  for (int i = 0; i < NUMBOTSPERTEAM; i++)
  {
    // If the bot has a line of the sight to another bot, plus if the distance
    // is below 600, marks as there is a enemy close and switch to the attack
    // state
    if (StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(),
      DynamicObjects::GetInstance()->GetBot(1, i).GetLocation()) &&
      (pBot->GetLocation() - DynamicObjects::GetInstance()->GetBot(1, i).
      GetLocation()).magnitude() <= 600 && DynamicObjects::GetInstance()->
      GetBot(1, i).IsAlive())
    {
      pBot->ChangeState(Attack::GetInstance());
    }
  }

} // IsEnemyClose()


bool Defend::StillOwnDP(Bot* pBot)
{ // Returns the result of a check to see if the domination point the bot is
  // defending is still owned by the bots team or not

  return (pBot->GetTeamNumber() == DynamicObjects::GetInstance()->
    GetDominationPoint(pBot->domTarget).m_OwnerTeamNumber);

} // StillOwnDP()


char* Defend::GetStateName()
{ // Returns the name of the state for debugging purposes

  return name;

} // GetStateName()


void Defend::Release()
{ // If pInst is valid, deletes and defines as nullptr

  if (pInst)
  {
    delete pInst;
    pInst = nullptr;
  }

} // Release()