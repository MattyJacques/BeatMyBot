// Title: Capture.h
// Purpose: Allows to bot to venture and take over a DP


#pragma once


#include "bot.h" // Definition of bot type


class Capture : public State
{

private:
  static Capture* pInst; // Holds the instance of the class
  char name[11];

  // Private constuctor to make sure only one instance, sets the name of the
  // state for debugging
  Capture();

public:
  // Returns the instance to the class, if one is not already created
  // creates a new instance and returns the address of that instance
  static Capture* GetInstance();

  // Gets the domination point target, then proceeds to generate a path
  // to that domination point, also sets initial behaviours for the state
  void Enter(Bot* pBot);

  // Checks how far away the bot is from the targeted domination point while
  // checking to see if there are any enemies around us to attack. Then checks
  // to see if the domination point hasn't already been captured by another
  // member of the team
  void Execute(Bot* pBot);

  //Tidies up after the state has completed the activities
  void Exit(Bot* pBot);

  // Sets the domination point target using the ID provided or will work a crude
  // target is the value of -1 is provided as the target parameter also calls
  // GeneratePath to store the path to the target
  void SetTarget(Bot* pBot, int target);

  // Checks to see how close the bot is to the target, if the bot has line of 
  // sight, it switches behaviours to seek from followPath.
  void CheckDistance(Bot* pBot);

  // Checks to see if an enemy bot is close to this bot, if so switches state
  // to attack
  void CheckForEnemy(Bot* pBot);

  // Checks the ownership of the domination point that the bot is supposed to
  // be capturing. If this team has already captured, then switch to the defence
  // state
  void CheckDPOwner(Bot* pBot);

  // Returns the name of the state, used for debugging purposes
  char* GetStateName();

  // Releases the instance of the class by deleting the pInst pointer
  static void Release();


};

