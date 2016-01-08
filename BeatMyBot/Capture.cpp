// Title: Capture.cpp
// Purpose: Allows to bot to venture and take over a DP


#include "Capture.h"
#include "Pathfinder.h"
#include "dynamicObjects.h"
#include "staticmap.h"


Capture* Capture::pInst = nullptr;


Capture::Capture()
{ // Private constuctor to make sure only one instance

  // Sets name for on-screen debugging
  sprintf_s(name, "Capturing");

} // Capture()


Capture* Capture::GetInstance()
{ // Returns the instance to the class, if one is not already created
  // creates a new instance and returns the address of that instance

  if (!pInst)
    pInst = new Capture;

  return pInst;

} // GetInstance()


void Capture::Enter(Bot* pBot)
{ // Gets the domination point target, then proceeds to generate a path
  // to that domination point, also sets initial behaviours for the state

  SetTarget(pBot, -1);
  pBot->SetBehaviours(0, 0, 0, 0, 0, 1, 1);

} // Enter()


void Capture::Execute(Bot* pBot)
{ // Checks how far away the bot is from the targeted domination point while
  // checking to see if there are any enemies around us to attack. Then checks
  // to see if the domination point hasn't already been captured by another
  // member of the team

  CheckDistance(pBot);
  CheckForEnemy(pBot);
  CheckDPOwner(pBot);

  pBot->SetAcceleration(pBot->Accumulate(DynamicObjects::GetInstance()->GetDominationPoint(pBot->domTarget).m_Location, Vector2D(0, 0),
    pBot->GetLocation(), pBot->GetVelocity(), pBot->GetPath()));

} // Execute()


void Capture::Exit(Bot* pBot)
{ //Tidies up after the state has completed the activities

} // Exit()


void Capture::SetTarget(Bot* pBot, int target)
{ // Sets the domination point target using the ID provided or will work a crude
  // target is the value of -1 is provided as the target parameter also calls
  // GeneratePath to store the path to the target

  // Set the target
  if (target != -1)
    pBot->domTarget = target;
  else
    pBot->domTarget = pBot->GetBotNumber() % 3;

  // Generate path to domination point
  pBot->SetPath(&Pathfinder::GetInstance()->GeneratePath(pBot->GetLocation(),
    DynamicObjects::GetInstance()->GetDominationPoint(pBot->domTarget).m_Location));

} // SetTarget()


void Capture::CheckDistance(Bot* pBot)
{ // Checks to see how close the bot is to the target, if the bot has line of 
  // sight, it switches behaviours to seek from followPath. 

  DynamicObjects* pDO = DynamicObjects::GetInstance();

  // If the bot can see the domination point, check to see if the bot is close.
  // If not close, then switch to seek
  if (StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(),
    DynamicObjects::GetInstance()->GetDominationPoint(pBot->domTarget).m_Location))
  {
      pBot->SetBehaviours(1, 0, 0, 0, 0, 1, 0);
      pBot->GetPath()->clear();
  }

} // CheckDistance()


void Capture::CheckForEnemy(Bot* pBot)
{ // Checks to see if an enemy bot is close to this bot, if so switches state
  // to attack

  // Loop through all enemy bots on the next team
  for (int i = 0; i < NUMBOTSPERTEAM; i++)
  {
    // If the bot has a line of the sight to another bot, plus if the distance
    // is below 100, marks as there is a enemy close and switch to the attack
    // state
    if (StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(), 
      DynamicObjects::GetInstance()->GetBot(1, i).GetLocation()) && 
      (pBot->GetLocation() - DynamicObjects::GetInstance()->GetBot(1, i).
      GetLocation()).magnitude() < 400)
    {
      int anInt = 1;
      // CHANGE WHEN THE ATTACK STATE IS COMPLETE
    }
  }

} // CheckForEnemy()


void Capture::CheckDPOwner(Bot* pBot)
{ // Checks the ownership of the domination point that the bot is supposed to
  // be capturing. If this team has already captured, then switch to the defence
  // state

  if (DynamicObjects::GetInstance()->GetDominationPoint(pBot->domTarget)
    .m_OwnerTeamNumber == pBot->GetTeamNumber())
  {
    int anInt = 1;
    //CHANGE WHEN DEFENCE STATE IS COMPLETE
  }

} // CheckDPOwner()


char* Capture::GetStateName()
{ // Returns the name of the state, used for debugging purposes

  return name;

} // GetStateName()


void Capture::Release()
{ // Releases the instance of the class by deleting the pInst pointer

  if (pInst)
  {
    delete pInst;
    pInst = nullptr;
  }

} // Release()