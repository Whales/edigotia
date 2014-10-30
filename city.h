#ifndef _CITY_H_
#define _CITY_H_

#include "area.h"
#include "building.h"
#include "citizen.h"
#include "map.h"
#include "military.h"
#include "race.h"

#include "window.h"
#include "cuss.h"
#include "glyph.h"    // For City::get_glyph()
#include <map>

// For determining City::get_glyph() - above this uses O, below uses o
#define CITY_POPULATION_LARGE 1000

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

// Return value when we attempt to enqueue an area
enum Area_queue_status
{
  AREA_QUEUE_OK,              // OK to build
  AREA_QUEUE_OUTSIDE_RADIUS,  // Outside control radius
  AREA_QUEUE_BAD_TERRAIN,     // Terrain won't let us build this area
  AREA_QUEUE_OCCUPIED,        // Location already contains an area
  AREA_QUEUE_MAX
};

// Return value when we attempt to enqueue a building
enum Building_queue_status
{
  BUILDING_QUEUE_OK,            // OK to build
  BUILDING_QUEUE_NO_RESOURCES,  // We lack the necessary resources
  BUILDING_QUEUE_MAX
};

enum City_type
{
  CITY_TYPE_NULL = 0,
  CITY_TYPE_CITY,
  CITY_TYPE_DUCHY,    // Capital of a ducy
  CITY_TYPE_CAPITAL,  // Capital of a kingdom
  CITY_TYPE_MAX
};

City_type lookup_city_type(std::string name);
std::string city_type_name(City_type type);

class City
{
public:
  City();
  virtual ~City();

  virtual bool is_player_city() { return false; }
  virtual bool is_ai_city()     { return true;  }

// General-data mutators
  virtual void start_new_city();  // Sets up our population and more!
  virtual void do_turn();
  void set_random_name();
  void set_race(Race new_race);
  void set_city_type(City_type new_type);

// General-data accessors
  int get_total_population(Citizen_type type = CIT_NULL);
  std::string get_name();
  glyph get_glyph();
  City_type get_city_type();
  Race get_race();

// Resource-related functions
  int get_resource_amount(Resource res);
  int get_mineral_amount (Mineral  min);
  int get_food_cap();         // Returns get_food_consumption() * 10 (days)
  int get_food_consumption(Citizen_type type = CIT_NULL); // Per day

  bool has_resource (Resource res, int amount);
  bool has_resource (Resource_amount res);
  bool has_resources(std::vector<Resource_amount> res_used);
  bool has_resources(std::map<Resource,int>       res_used);

  bool has_mineral (Mineral res, int amount);
  bool has_mineral (Mineral_amount res);
  bool has_minerals(std::vector<Mineral_amount> min_used);
  bool has_minerals(std::map<Mineral,int>       min_used);

  bool expend_resource (Resource res, int amount);
  bool expend_resource (Resource_amount res);
  bool expend_resources(std::vector<Resource_amount> res_used);
  bool expend_resources(std::map<Resource,int>       res_used);

  bool expend_mineral (Mineral res, int amount);
  bool expend_mineral (Mineral_amount res);
  bool expend_minerals(std::vector<Mineral_amount> min_used);
  bool expend_minerals(std::map<Mineral,int>       min_used);

  void gain_resource (Resource res, int amount);
  void gain_resource (Resource_amount res);
  void gain_resources(std::vector<Resource_amount> res_used);
  void gain_resources(std::map<Resource,int>       res_used);

// Public data
  Citizens population[CIT_MAX];

  int resources[RES_MAX];
  int minerals[MINERAL_MAX];

protected:
  std::string name;
  City_type type;
  Race race;
};

#endif
