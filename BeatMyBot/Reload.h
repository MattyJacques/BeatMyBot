// Title        : Reload.h
// Purpose      : Handles the bot being able to find a supply point for reload


#pragma once


#include "bot.h"


class Reload : public State
{

private:
  static Reload* pInstance;  // Holds the instance of the class

  // Private constructor to avoid unauth access
  Reload();
public:
  char name[11];

  // Returns the instance of the class, if none currently exists create one
  static Reload* GetInstance();

  // Sets behaviours for the capture state
  void Enter(Bot* pBot);

  // Checks to see if there is a lineofsight to a reload point, if so switches 
  // behaviours otherwise just continue following path
  void Execute(Bot* pBot);

  // Tidies up after the state activity has completed
  void Exit(Bot* pBot);

  // Calls GeneratePath to get a path to desired reload point, then sets 
  //the bots path to the generated path
  void GetPath(Bot* pBot);

  // Returns index of cloest supply point
  int GetClosestSupply(Vector2D pos);

  // Sets the target and creates the path
  void SetTarget(Bot* pBot);

  // Checks to see if the bots needs to switch to arrive
  void SetBehaviours(Bot* pBot);

  // Returns the name of the state
  char* GetStateName();

  // If pInstance is valid, deletes the instance then defines al nullptr
  static void Release();
};

