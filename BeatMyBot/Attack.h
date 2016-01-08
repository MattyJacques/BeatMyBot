// Title: Attack.h
// Purpose: Allows to bot to find a target and shoot


#pragma once


#include "bot.h"  /// Definition of the bot object


class Attack : public State
{

private:
  static Attack* pInst;     // Holds the instance of the class
  char name[11];            // Name of the class for debugging

  // Private constructor for singleton, sets name of the class for debugging
  Attack();

public:
  // Returns the current instance of the class, if one does not currently
  // exist, creates on then returns pInst
  static Attack* GetInstance();

  // Calculates the target that the bot is going to be attacking and sets
  // behaviours to pursue and avoid walls
  void Enter(Bot* pBot);

  // Checks for the line of the sight between the bot and the enemy, checks
  // ammo and checks for enemies health to see if the bot can leave the
  // attacking state
  void Execute(Bot* pBot);

  // Tidies up after the states activities, getting ready for the next state
  void Exit(Bot* pBot);

  // Searches for an enemy bot within range and in line of sight, sets the bots
  // target to the ID of the bot found and starts aiming
  void GetTarget(Bot* pBot);

  // Checks if there is a line of sight between the bot and the target bot
  bool HaveLOS(Bot* pBot);

  // Checks to see if the target is dead
  bool TargetDead(Bot* pBot);

  // Returns the distance inbetween the bot and an enemy
  float DistanceToEnemy(Bot* pBot);

  // Returns the name of the state for debugging purposes
  char* GetStateName();

  // If called when pInst is valid, deletes the instance and defines as nullptr
  static void Release();
};

