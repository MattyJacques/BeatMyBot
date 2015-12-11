#pragma once

#include "State.h"

class Attack : public State
{

private:
  static Attack* pInstance;
  Bot* targetBot;

  Attack();

public:
  static Attack* GetInstance();
  void Enter(Bot* pBot);
  void Execute(Bot* pBot);
  void Exit(Bot* pBot);
  ~Attack();

};

