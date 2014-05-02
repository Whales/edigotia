#ifndef _CITY_H_
#define _CITY_H_

#include "citizen.h"
#include "building.h"
#include "area.h"
#include "map.h"
#include "window.h"
#include "cuss.h"

class City
{
public:
  City();
  ~City();

  int population[CIT_MAX];
  int buildings[BUILD_MAX];

  std::vector<Area> areas;

  City_map map;

  void place_keep();

  void interface_buildings();
  void draw_map(Window* w, cuss::interface* i_map, bool interactive = false);
};

#endif
