#include "race.h"
#include "stringfunc.h"

Race_datum::Race_datum()
{
  uid = -1;
  name        = "Uninitialized Race";
  plural_name = "Uninitialized Race";
  adjective   = "Uninitialized Race";

  color = c_ltgray;

// Some reasonable defaults.
  for (int i = 0; i < CIT_MAX; i++) {
    starting_population[i]  = 0;
    birth_rate[i]           = 100;
    citizen_ratio[i]        = 0;
    morale_requirement[i]   = 0;
    low_tax_rate[i]         = 10;
    high_tax_rate[i]        = 70;
  }

// Some special cases...
  citizen_ratio[CIT_MERCHANT] = 10;
  citizen_ratio[CIT_BURGHER]  = 10;
  morale_requirement[CIT_MERCHANT] = 50;
  morale_requirement[CIT_BURGHER]  = 50;

  food_consumption = 100;

  for (int i = 0; i < SKILL_MAX; i++) {
    skill_level[i] = 3;
  }
}

Race_datum::~Race_datum()
{
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
    case SKILL_MINING:    return "mining";
    case SKILL_FORESTRY:  return "forestry";
    case SKILL_MAX:       return "BUG - SKILL_MAX";
    default:              return "BUG - Unnamed Race_skill";
  }
  return "BUG - Escaped race_skill_name() switch";
}
