#pragma once

#include "State.h"
#include "bot.h"


class Attack : public State
{

private:
  static Attack* pInstance;

  Attack();

public:
  static Attack* GetInstance();
  void Enter(Bot* pBot);
  void Execute(Bot* pBot);
  void GetTarget(Bot* pBot);
  void Exit(Bot* pBot);
  static void Release();
  ~Attack();

};

