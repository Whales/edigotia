#include "ai_city.h"
#include "window.h" // For debugmsg()

AI_city::AI_city()
{
  type = CITY_TYPE_CITY;
  race = RACE_NULL;
  for (int i = 0; i < CIT_MAX; i++) {
    population[i].type = Citizen_type(i);
  }

  population[CIT_PEASANT].add_citizens(150);
}

AI_city::~AI_city()
{
}

// TODO: This is a duplicate of City::get_total_population().  They should
//       probably both be children of the same parent class.
// type defaults to CIT_NULL
