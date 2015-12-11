#pragma once


#include "State.h"


class Hunt
{

private:
  static Hunt* pInstance;

  Hunt();

public:
  static Hunt* GetInstance();
  void Enter(Bot* pBot);
  void Execute(Bot* pBot);
  void Exit(Bot* pBot);
  ~Hunt();
};

