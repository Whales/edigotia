#ifndef _CITY_H_
#define _CITY_H_

#include "citizen.h"
#include "building.h"
#include "area.h"
#include "map.h"
#include "window.h"
#include "cuss.h"
#include <map>

struct Citizens
{
  int count;
  int wealth;

  Citizens() { count = 0; wealth = 0; }
};

enum Area_queue_status
{
  AREA_QUEUE_OK,              // OK to build
  AREA_QUEUE_OUTSIDE_RADIUS,  // Outside control radius
  AREA_QUEUE_BAD_TERRAIN,     // Terrain won't let us build this area
  AREA_QUEUE_OCCUPIED,        // Location already contains an area
  AREA_QUEUE_MAX
};

class City
{
public:
  City();
  ~City();

// Starting functions
  bool place_keep();

// Display output
  void interface_buildings();
  void draw_map(cuss::element* e_draw, Point sel, bool radius_limited = false);
  void display_map(Window* w, cuss::interface* i_map, bool interactive = false,
                   bool radius_limited = false);

// Standard turn functions
  void do_turn();

// Changes
  Area_queue_status add_area_to_queue(Area_type type, Point location);
  Area_queue_status add_area_to_queue(Area area);

  bool expend_resources(std::vector<Resource_amount> resources);
  bool expend_resources(std::map<Resource,int> resources);

// Data functions
  bool inside_radius(int x, int y);
  bool inside_radius(Point p);

  Area* area_at(int x, int y);
  Area* area_at(Point p);

  int get_total_population();

// Data
  Citizens population[CIT_MAX];
  //int population[CIT_MAX];
  int open_buildings[BUILD_MAX];
  int closed_buildings[BUILD_MAX];
  int resources[RES_MAX];

  int radius;

  std::vector<Area> areas;
  std::vector<Area> area_queue;
  int labor_pool;

  City_map map;

};

#endif
