#pragma once

#include "State.h"
#include "bot.h"

class Attack : public State
{

private:
  static Attack* pInstance;
  Bot* target;

  Attack();

public:
  static Attack* GetInstance();
  void Enter(Bot* pBot, Bot* target);
  void Execute(Bot* pBot);
  void Exit(Bot* pBot);
  ~Attack();

};

