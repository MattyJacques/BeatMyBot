// Title        : Defend.h
// Purpose      : Allows AI to defend a DP


#pragma once


#include "State.h"      // For inheritance


class Defend : public State
{

private:
  static Defend* pInstance;   // Holds the instance of the class

  // Private constructor to avoid unauth access
  Defend();

public:
  // Returns the instance of the class, if none currently exists create one
  static Defend* GetInstance();

  // Sets behaviours for defend state
  void Enter(Bot* pBot);

  // DOES SHIT
  void Execute(Bot* pBot);

  // Tidies up after the state activity is completed
  void Exit(Bot* pBot);

  // If called while pInstance is valid, deletes and defines as nullptr
  static void Release();
};

