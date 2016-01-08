#include "Hunt.h"

Hunt::Hunt()
{
  sprintf_s(name, "Hunting");
}

char* Hunt::GetStateName()
{
  return name;
}
