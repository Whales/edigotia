#include "animal.h"
#include "stringfunc.h" // search_for_animal() needs trim() and no_caps()

Animal_datum::Animal_datum()
{
  name = "NULL Animal";

  min_temp      =   0;
  max_temp      = 100;
  min_altitude  =   0;
  max_altitude  = 100;
  min_rainfall  =   0;
  max_rainfall  = 100;
  percentage    =   0;

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

Animal search_for_animal(std::string name)
{
  name = no_caps( trim(name) );
  for (int i = 1; i < ANIMAL_MAX; i++) {
    Animal ret = Animal(i);
    if (Animal_data[ret]->name.find(name) != std::string::npos) {
      return ret;
    }
  }
  return ANIMAL_NULL;
}

