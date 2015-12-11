#pragma once

class Bot;

class State
{

protected:
  static State* pInstance;

  State();

public:
  static State* GetInstance();
  void Enter(Bot* pBot);
  void Execute(Bot* pBot);
  void Exit(Bot* pBot);
  static void Release();
  ~State();
};

