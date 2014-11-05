#include "race.h"
#include "stringfunc.h"
#include "rng.h"    // For Race_datum::get_city_name()
#include "window.h" // For debugmsg()
#include <cstdarg>  // For variadic function Race_datum::add_city_names()
#include <sstream>  // For Race_datum::get_city_name()

Race_datum::Race_datum()
{
  uid = -1;
  name        = "Uninitialized Race";
  plural_name = "Uninitialized Race";
  adjective   = "Uninitialized Race";

  color = c_ltgray;

// Some reasonable defaults.
  for (int i = 0; i < CIT_MAX; i++) {
    starting_population [i] = 0;
    birth_rate          [i] = 100;
    citizen_ratio       [i] = 0;
    morale_requirement  [i] = 0;
    low_tax_rate        [i] = 10;
    high_tax_rate       [i] = 70;
  }

  for (int i = 0; i < RES_MAX; i++) {
    starting_resources[i] = 0;
  }

// Some special cases...
  citizen_ratio[CIT_MERCHANT] = 10;
  citizen_ratio[CIT_BURGHER]  = 10;
  morale_requirement[CIT_MERCHANT] = 50;
  morale_requirement[CIT_BURGHER]  = 50;

  for (int i = 0; i < SKILL_MAX; i++) {
    skill_level[i] = 3;
  }

// Non-array-based variables
  cluster_min = 3;
  cluster_max = 4;
  base_combat = 10;
  hp = 100;
  food_consumption = 100;

}

Race_datum::~Race_datum()
{
}

void Race_datum::add_city_names(std::string pos, ...)
{
  std::string fixed_pos = no_caps( trim( pos ) );
  std::vector<std::string>* names;
  if (fixed_pos == "start") {
    names = &city_name_start;
  } else if (fixed_pos == "middle") {
    names = &city_name_middle;
  } else if (fixed_pos == "end") {
    names = &city_name_end;
  } else {
    debugmsg("Race_datum::add_city_names(\"%s\") called!", fixed_pos.c_str());
    return;
  }

// Start our variadic list and all that
  va_list ap;
  va_start(ap, pos);
  char* tmp;
  bool done = false;
  while (!done) {
    tmp = va_arg(ap, char*);
    if (tmp == NULL) {
      done = true;
    } else {
      names->push_back( std::string(tmp) );
    }
  }
}

std::string Race_datum::get_city_name()
{
  std::stringstream ret;
  if (!city_name_start.empty()) {
    int i = rng(0, city_name_start.size() - 1);
    ret << city_name_start[i];
  }
  if (!city_name_middle.empty()) {
    int i = rng(0, city_name_middle.size() - 1);
    ret << city_name_middle[i];
  }
  if (!city_name_end.empty()) {
    int i = rng(0, city_name_end.size() - 1);
    ret << city_name_end[i];
  }

  return capitalize( ret.str() );
}

Race_skill lookup_race_skill(std::string name)
{
  name = no_caps( trim( name ) );
  for (int i = 0; i < SKILL_MAX; i++) {
    Race_skill ret = Race_skill(i);
    if (name == no_caps( trim( race_skill_name(ret) ) ) ) {
      return ret;
    }
  }
  return SKILL_NULL;
}

std::string race_skill_name(Race_skill skill)
{
  switch (skill) {
    case SKILL_NULL:      return "NULL";
    case SKILL_FARMING:   return "farming";
    case SKILL_HUNTING:   return "hunting";
    case SKILL_LIVESTOCK: return "livestock";
    case SKILL_MINING:    return "mining";
    case SKILL_FORESTRY:  return "forestry";
    case SKILL_MAX:       return "BUG - SKILL_MAX";
    default:              return "BUG - Unnamed Race_skill";
  }
  return "BUG - Escaped race_skill_name() switch";
}
