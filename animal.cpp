#include "animal.h"
#include "stringfunc.h" // search_for_animal() needs trim() and no_caps()
#include "rng.h"
#include <sstream>

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
  hp = 50;
  flee_chance = 100;
  pack_chance = 0;
  max_pack_size = 1;

  reproduction_rate =   30;
  hardiness         = 2000;

  food_killed     = 0;
  food_livestock  = 0;
  carnivore       = false;

  tameness        = 0;
  size            = 1;
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

nc_color animal_action_color(Animal_action action)
{
  switch (action) {
    case ANIMAL_ACT_NULL:     return c_dkgray;
    case ANIMAL_ACT_FLEE:     return c_cyan;
    case ANIMAL_ACT_KILL:     return c_red;
    case ANIMAL_ACT_CAPTURE:  return c_green;
    case ANIMAL_ACT_MAX:      return c_dkgray;
    default:                  return c_magenta;
  }
  return c_magenta;
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

// range defaults to false.
std::string animal_amount_ranking(int amt, bool range)
{
  if (amt == 0) {
    return (range ? "0" : "no");

  } else if (amt <= 5) {
    return (range ? "1 - 5" : "a few");

  } else if (amt <= 15) {
    return (range ? "6 - 15" : "a pack of");

  } else if (amt <= 50) {
    return (range ? "16 - 50" : "a small colony of");

  } else if (amt <= 300) {
    return (range ? "51 - 300" : "a colony of");

  } else if (amt <= 750) {
    return (range ? "301 - 750" : "a large colony of");

  } else if (amt <= 1500) {
    return (range ? "751 - 1500" : "an abundance of");

  } else if (amt <= 3000) {
    return (range ? "1501 - 3000" : "thousands of");

  }

  return (range ? "Over 3000" : "uncountable");
}

std::string animal_danger_ranking(int danger)
{
  if (danger == 0) {
    return "<c=ltblue>Harmless<c=/>";
  }

  if (danger <= 10) {
    return "<c=ltblue>Mild<c=/>";
  }

  if (danger <= 20) {
    return "<c=yellow>Moderate<c=/>";
  }

  if (danger <= 30) {
    return "<c=red>High<c=/>";
  }

  if (danger <= 40) {
    return "<c=red>Deadly<c=/>";
  }

  return "<c=ltred>Nightmarish<c=/>";
}

// verb defaults to ""
std::string list_animals(std::map<Animal,int> animals, std::string verb)
{
  std::stringstream ret;

  int size = animals.size();
  int cur = 0;

  for (std::map<Animal,int>::iterator it = animals.begin();
       it != animals.end();
       it++) {
    Animal type = it->first;
    int amount  = it->second;
    cur++;
// Conjunction (or comma)
    Animal_datum* animal_dat = Animal_data[ type ];
    if (cur > 1) {
      if (cur == size) {
        ret << " and ";
      } else {
        ret << ", ";
      }
    }

// Article (or number)
    if (amount == 1) {
      if (!animal_dat->name.empty() && is_vowel(animal_dat->name[0])) {
        ret << "an ";
      } else {
        ret << "a ";
      }
      ret << animal_dat->name;
    } else {
      ret << amount << " " << animal_dat->name_plural;
    }

  } // iterate over animals

// Verb
  if (!verb.empty()) {
    if (size > 1 || animals.begin()->second > 1) {
      ret << " were ";
    } else {
      ret << " was ";
    }
    ret << verb;
  }

  return ret.str();
}
