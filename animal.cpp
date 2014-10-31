#include "animal.h"

Animal_datum::Animal_datum()
{
  name = "NULL Animal";

  danger = 0;

  reproduction_rate =   30;
  hardiness         = 2000;

  food_killed     = 0;
  food_livestock  = 0;
  food_eaten      = 0;
  carnivore       = false;

  tameness        = 0;
}

Animal_datum::~Animal_datum()
{
}
