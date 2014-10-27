#include "race.h"

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
