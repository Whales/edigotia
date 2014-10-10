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
  Citizens();
  ~Citizens();

  Citizen_type type;

  int count;
  int employed;
  int wealth;
  int tax_morale;
  int morale_points;
  int starvation;

  int get_unemployed();
  int get_income();
  int get_morale_percentage();
  int get_starvation_chance();

  void add_citizens   (int amount);
  void remove_citizens(int amount);
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
  void draw_map(cuss::element* e_draw, Point sel = Point(-1, -1),
                bool radius_limited = false, bool only_terrain = false);

// Standard turn functions
  void do_turn();

// Changes
  Area_queue_status add_area_to_queue(Area_type type, Point location);
  Area_queue_status add_area_to_queue(Area area);
  void add_open_area(Area area);  // Move it from queue to areas.

  bool expend_resource(Resource res, int amount);
  bool expend_resources(std::vector<Resource_amount> resources);
  bool expend_resources(std::map<Resource,int> resources);

  bool employ_citizens(Citizen_type type, int amount, Building* job_site);
  bool fire_citizens  (Citizen_type type, int amount, Building* job_site);

// Data functions

// Map-related functions
  bool inside_radius(int x, int y);
  bool inside_radius(Point p);

  Area* area_at(int x, int y);
  Area* area_at(Point p);

  std::string get_map_info(Point p);

// Citizens/population/housing-related functions
  int get_total_population(Citizen_type type = CIT_NULL);
// Returns the number of citizens in the given class currently without a job.
  int get_unemployed_citizens(Citizen_type type = CIT_NULL);

  int get_total_housing(Citizen_type type = CIT_NULL);

  int get_military_count();
  int get_military_supported();

// Gain this many birth points each turn.  Based mostly on population.  When our
// birth points reaches 100, a new citizen is born.
  int get_daily_birth_points();
/* The number of citizens of the class below this required for a citizen of this
 * class.  For instance, if get_required_ratio(CIT_MERCHANT) is 20, then we need
 * 20 peasants before a merchant can be born.
 */
  int get_required_ratio(Citizen_type cit_type);
/* When a new citizen is born, we need to choose its type; this function returns
 * the chance that its type will be cit_type.  The chances are independant of
 * each other; they may total to more than 100%.
 */
  int get_chance_to_birth(Citizen_type cit_type);
  void birth_citizen(); // Create a new citizen, including picking its type

// Building/Area-related functions
  std::vector<Building*> get_all_buildings();
  int get_number_of_buildings(Building_type type = BUILD_NULL);
  int get_number_of_areas    (Area_type     type = AREA_NULL);
  int get_total_maintenance();
  int get_fields_worked();
  int get_empty_fields();
  int get_shafts_worked();
  int get_free_shafts();

// Resource-related functions
  int get_resource_amount(Resource res);
  int get_mineral_amount (Mineral  min);

  // General finance
  int get_total_wages(Citizen_type type = CIT_NULL);
  int get_military_expense();
  void set_tax_rate(Citizen_type type, int rate);
  int get_low_tax_rate (Citizen_type type);
  int get_high_tax_rate(Citizen_type type);
  int get_taxes(Citizen_type type = CIT_NULL);
  int get_corruption_percentage();
  int get_corruption_amount();

  // Crops & food
  int get_food_consumption(Citizen_type type = CIT_NULL);
  int get_food_production();
  std::vector<Crop_amount> get_crops_grown();

  // Mining
  int get_amount_mined(Mineral mineral = MINERAL_NULL);
  std::map<Mineral,int> get_minerals_mined();
  std::map<Mineral,int> get_minerals_used();

// Trade-related functions
  int get_import(Resource res);
  int get_export(Resource res);

// Data
  Citizens population[CIT_MAX];
  int tax_rate[CIT_MAX];

  int birth_points; // We gain these each turn; at 100, a citizen is born.

  std::vector<Military_unit> units_stationed;

  int resources[RES_MAX];
  int minerals[MINERAL_MAX];

  int radius;

  std::vector<Building> buildings;
  std::vector<Area> areas;
  std::vector<Area> area_queue;

  City_map map;

};

#endif
