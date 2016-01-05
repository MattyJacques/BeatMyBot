// Title        : Hunt.h
// Purpose      : Allows AI to hunt an enemy bot


#pragma once


#include "State.h"          // For inheritance


class Hunt
{

private:
  static Hunt* pInstance;   // Holds the instance of the class

  // Private constructor to avoid unauth access
  Hunt();

public:
  // Returns the instance of the class, if none currently exists create one
  static Hunt* GetInstance();

  // Sets behaviours for the hunt state
  void Enter(Bot* pBot);

  // DO SHIT
  void Execute(Bot* pBot);

  // Tidies up after the state activity is completed
  void Exit(Bot* pBot);
  
  // If called while pInstance is valid, deletes and defines as nullptr
  static void Release();
};

