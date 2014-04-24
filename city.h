#ifndef _CITY_H_
#define _CITY_H_

#include "citizen.h"
#include "building.h"

class City
{
  City();
  ~City();

  int population[CIT_MAX];

  int buildings[BUILD_MAX];

  void interface_buildings();
};

#endif
