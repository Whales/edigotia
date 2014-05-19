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

  int radius;

  std::vector<Area> areas;
  std::vector<Area> area_queue;
  int labor_pool;

  City_map map;

  bool place_keep();

  void interface_buildings();
  void draw_map(Window* w, cuss::interface* i_map, bool interactive = false,
                bool radius_limited = false);

  void do_turn();
  void add_area_to_queue(Area_type type, Point location);
  void add_area_to_queue(Area area);
};

#endif
