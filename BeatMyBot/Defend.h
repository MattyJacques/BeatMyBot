#pragma once


#include "State.h"


class Defend : public State
{

private:
  static Defend* pInstance;

  Defend();

public:
  static Defend* GetInstance();
  void Enter(Bot* pBot);
  void Execute(Bot* pBot);
  void Exit(Bot* pBot);
  ~Defend();
};

