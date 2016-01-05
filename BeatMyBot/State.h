// Title        : State.h
// Purpose      : Template state for inheritance


#pragma once

class Bot;

class State
{
public:
  virtual void Enter(Bot* pBot) = 0;
  virtual void Execute(Bot* pBot) = 0;
  virtual void Exit(Bot* pBot) = 0;
};

