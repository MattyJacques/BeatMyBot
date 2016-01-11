// Title        : State.h
// Purpose      : Template state for inheritance


#pragma once

template <class Entity>

class State
{
public:
  virtual void Enter(Entity* entity) = 0;
  virtual void Execute(Entity* entity) = 0;
  virtual void Exit(Entity* entity) = 0;
  virtual char* GetStateName() = 0;
};

