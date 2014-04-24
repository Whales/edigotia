#ifndef _CITY_H_
#define _CITY_H_

#include "citizen.h"
#include "building.h"

class City
{
  int population[CIT_MAX];

  std::vector<Building> buildings;
};


#endif
