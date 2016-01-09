// Title: Defend.h
// Purpose: Controls the bot while defending a domination point


#pragma once


#include "bot.h"


class Defend : public State
{

private:
  static Defend* pInst;   // Holds the instance of the class
  char name[11];          // Holds the name for debugging

  // Private constructor for singleton, sets name of state for debugging
  Defend();

public:
  // Returns the current instance of the class, if none currently exists,
  // create one and return pInst
  static Defend* GetInstance();

  // Sets the domination point the bot is defending then proceeds to ask for
  // the path to that domination point setting the path in bot. Sets the 
  // behaviours to wall avoid and follow path
  void Enter(Bot* pBot);

  // Checks to see if we are close to the domination point the bot is defending
  // Checks to see if there are any enemy bots within range to attack then makes
  // sure the bots team still owns the domination point
  void Execute(Bot* pBot);

  // Tidies up after the activities of the state
  void Exit(Bot* pBot);

  // Sets the target the bots needs to defend, calls to generate the path to
  // the target
  void SetTarget(Bot* pBot);

  // Checks the distance from the bot to the domination point, if the bot has
  // line of sight to the domination point, switch to seek, if close to the 
  // domination point, switch to arrive
  void CheckDistance(Bot* pBot);

  // Checks to see if there are enemy bots in range of the domination point the
  // bot is defending, if so switches to attack
  void IsEnemyClose(Bot* pBot);

  // Returns the result of a check to see if the domination point the bot is
  // defending is still owned by the bots team or not
  bool StillOwnDP(Bot* pBot);

  // Returns the name of the state for debugging purposes
  char* GetStateName();

  // If pInst is valid, deletes and defines as nullptr
  static void Release();
};

