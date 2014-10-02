#ifndef _CITY_H_
#define _CITY_H_

#include "citizen.h"
#include "military.h"
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
  void add_open_area(Area area);  // Move it from queue to areas.

  bool expend_resources(std::vector<Resource_amount> resources);
  bool expend_resources(std::map<Resource,int> resources);

// Data functions

// Map-related functions
  bool inside_radius(int x, int y);
  bool inside_radius(Point p);

  Area* area_at(int x, int y);
  Area* area_at(Point p);

// Population/housing-related functions
  int get_total_population(Citizen_type type = CIT_NULL);
  int get_total_housing(Citizen_type type = CIT_NULL);

  int get_military_count();
  int get_military_supported();

// Building/Area-related functions
  int get_number_of_buildings(Building_type type = BUILD_NULL);
  int get_number_of_areas    (Area_type     type = AREA_NULL);
  int get_fields_worked();
  int get_empty_fields();

// Resource-related functions
  int get_resource_amount(Resource res);
  int get_food_consumption(Citizen_type type = CIT_NULL);
  int get_food_production();

  std::vector<Crop_amount> get_crops_grown();

// Trade-related functions
  int get_import(Resource res);
  int get_export(Resource res);

// Data
  Citizens population[CIT_MAX];
  std::vector<Military_unit> units_stationed;

  int open_buildings[BUILD_MAX];
  int closed_buildings[BUILD_MAX];
  int resources[RES_MAX];

  int radius;

  std::vector<Building> buildings;
  std::vector<Area> areas;
  std::vector<Area> area_queue;
  int labor_pool;

  City_map map;

};

#endif
