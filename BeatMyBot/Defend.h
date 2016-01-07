// Title        : Defend.h
// Purpose      : Allows AI to defend a DP


#pragma once


#include "bot.h"      // Definition of bot plus State


class Defend : public State
{

private:
  static Defend* pInstance;   // Holds the instance of the class

  // Private constructor to avoid unauth access
  Defend();

public:
  char name[11];

  // Returns the instance of the class, if none currently exists create one
  static Defend* GetInstance();

  // Sets behaviours for defend state
  void Enter(Bot* pBot);

  // Checks if we have a target to defend, if not chooses the closest DP. 
  // Travels to DP then waits for enemy to get close to DP and switches to
  // attack.
  void Execute(Bot* pBot);

  // Tidies up after the state activity is completed
  void Exit(Bot* pBot);

  // Set bots DP target to given value, if value is null find closest DP, also
  // gets path to the target and sets to bots path
  void SetTarget(Bot* pBot, int target);

  // Returns the closest DP to the bots location
  int GetClosestDP(Bot* pBot);

  // Checks all enemy bots to see if any are close to the DP we are defending
  void IsEnemyClose(Bot* pBot);

  char* GetStateName();

  // If called while pInstance is valid, deletes and defines as nullptr
  static void Release();
};

