// Title        : Attack.h
// Purpose      : Allows AI to attack an enemy bot


#pragma once


#include "State.h"  // For inheritance
#include "bot.h"    // For definition of bot type


class Attack : public State
{
private:
  static Attack* pInstance;  // Holds the instance of the class

  // Private constructor to avoid unauth access
  Attack();

public:
  // Returns the instance of the class, if none currently exists create one
  static Attack* GetInstance();

  // Sets behaviours for the attack state
  void Enter(Bot* pBot);

  // Checks for line of sight to enemy bot, if so switches behaviours. If close
  // enough, aims at bot and fires is accuracy check passes
  void Execute(Bot* pBot);

  // PROBABLY NOT NEEDED OR WILL NEED TO BE CHANGED
  void GetTarget(Bot* pBot);

  // Tidies up after the state activity is completed
  void Exit(Bot* pBot);

  // If called while pInstance is valid, deletes and defines as nullptr
  static void Release();

};

