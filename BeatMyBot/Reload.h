// Title: Reload.h
// Purpose: Allows to bot to venture and resupply at one of the points


#pragma once


#include "bot.h"        // Definition of bot


class Reload : public State
{

private:
  static Reload* pInst;  // Holds the instance of the class
  char name[11];         // Name of the class for debugging

  // Private constructor for singleton, sets the name of the class
  Reload();

public:
  // Returns the instance of the class, if no current instance, creates one
  // and returns pInst
  static Reload* GetInstance();

  // Calculates the closest supply point and gets a path to that supply
  // point. Turns on wall avoid and followPath
  void Enter(Bot* pBot);

  // Checks the distance to the supply point while checking for line of sight
  // to change behaviours, checks ammo to make sure we haven't already 
  // resupplied and sets the acceleration for the bot
  void Execute(Bot* pBot);

  // Tidies up after the activities of the state
  void Exit(Bot* pBot);

  // Gets the closest resupply point and gets the path to that supply point
  void SetTarget(Bot* pBot);

  // Checks to see if the bots behaviours needs to switch to arrive or seek
  void SetBehaviours(Bot* pBot);

  // Returns the state name from debugging
  char* GetStateName();

  // If pInst is valid, deletes and sets to nullptr
  static void Release();

};

