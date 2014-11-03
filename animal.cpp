#include "animal.h"
#include "stringfunc.h" // search_for_animal() needs trim() and no_caps()
#include "rng.h"

bool Animal_amount::is_infinite()
{
  return (amount == INFINITE_RESOURCE);
}

Animal_amount Animal_amount::make_small()
{
  Animal_amount ret;
  ret.type = type;
  ret.amount = rng( amount * .1, amount * .2);
  return ret;
}

Animal_amount Animal_amount::randomize()
{
  Animal_amount ret;
  ret.type = type;
  ret.amount = rng( amount * .8, amount * 1.2);
  return ret;
}

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

  default_action = ANIMAL_ACT_KILL;
  danger = 0;
  flee_chance = 100;
  pack_chance = 0;
  max_pack_size = 1;

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

Animal_action lookup_animal_action(std::string name)
{
  name = no_caps( trim( name ) );
  for (int i = 0; i < ANIMAL_ACT_MAX; i++) {
    Animal_action ret = Animal_action(i);
    if (name == no_caps( animal_action_name(ret) ) ) {
      return ret;
    }
  }
  return ANIMAL_ACT_NULL;
}

std::string animal_action_name(Animal_action action)
{
  switch (action) {
    case ANIMAL_ACT_NULL:     return "NULL";
    case ANIMAL_ACT_FLEE:     return "flee";
    case ANIMAL_ACT_KILL:     return "kill";
    case ANIMAL_ACT_CAPTURE:  return "capture";
    case ANIMAL_ACT_MAX:      return "BUG - ANIMAL_ACT_MAX";
    default:                  return "BUG - Unnamed Animal_action";
  }
  return "BUG - Escaped animal_action_name() switch!";
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

std::string animal_amount_ranking(Animal_amount amt)
{
  if (amt.amount <= 5) {
    return "a few";

  } else if (amt.amount <= 15) {
    return "a pack of";

  } else if (amt.amount <= 50) {
    return "a small colony of";

  } else if (amt.amount <= 300) {
    return "a colony of";

  } else if (amt.amount <= 750) {
    return "a large colony of";

  } else if (amt.amount <= 1500) {
    return "an abundance of";

  } else if (amt.amount <= 3000) {
    return "thousands of";

  }

  return "uncountable";
}
