#ifndef _CITY_H_
#define _CITY_H_

#include "citizen.h"
#include "building.h"
#include "map.h"

class City
{
  City();
  ~City();

  int population[CIT_MAX];
  int buildings[BUILD_MAX];

  City_map map;

  void interface_buildings();
};

#endif
