#pragma once

#include "State.h"
#include "bot.h"

class Capture : public State
{

private:
  static Capture* pInstance;

  Capture();

public:
  static Capture* GetInstance();
  void Enter(Bot* pBot);
  void Execute(Bot* pBot);
  void Exit(Bot* pBot);
  void GetPath(Bot* pBot);
  void SetTarget(Bot* pBot, DominationPoint* target);
  static void Release();
  ~Capture();

};

