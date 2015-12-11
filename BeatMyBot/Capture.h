#pragma once

#include "State.h"

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
  ~Capture();

};

