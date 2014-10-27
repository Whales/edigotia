#include "race.h"
#include "stringfunc.h"

Race_datum::Race_datum()
{
  uid = -1;
  name = "Uninitialized Race";

// Some reasonable defaults.
  for (int i = 0; i < CIT_MAX; i++) {
    starting_population[i]  = 0;
    birth_rate[i]           = 100;
    citizen_ratio[i]        = 10;
    morale_requirement[i]   = 50;
    low_tax_rate[i]         = 10;
    high_tax_rate[i]        = 70;
  }

  food_consumption = 100;

  for (int i = 0; i < SKILL_MAX; i++) {
    skill_level[i] = 3;
  }
}

Race_datum::~Race_datum()
{
}

Species_skill lookup_species_skill(std::string name)
{
  name = no_caps( trim( name ) );
  for (int i = 0; i < SKILL_MAX; i++) {
    Species_skill ret = Species_skill(i);
    if (name == no_caps( trim( species_skill_name(ret) ) ) ) {
      return ret;
    }
  }
  return SKILL_NULL;
}

std::string species_skill_name(Species_skill skill)
{
  switch (skill) {
    case SKILL_NULL:      return "NULL";
    case SKILL_FARMING:   return "farming";
    case SKILL_MINING:    return "mining";
    case SKILL_FORESTRY:  return "forestry";
    case SKILL_MAX:       return "BUG - SKILL_MAX";
    default:              return "BUG - Unnamed Species_skill";
  }
  return "BUG - Escaped species_skill_name() switch";
}
