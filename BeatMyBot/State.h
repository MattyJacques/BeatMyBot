#pragma once

class Bot;

class State
{

protected:
  //static State* pInstance;

  //State();

public:
  //virutal static State* GetInstance() = 0;
  virtual void Enter(Bot* pBot) = 0;
  virtual void Execute(Bot* pBot) = 0;
  virtual void Exit(Bot* pBot) = 0;
  //static void Release();
  //~State();
};

