#ifndef _CITY_H_
#define _CITY_H_

#include "area.h"
#include "building.h"
#include "citizen.h"
#include "map.h"
#include "military.h"
#include "game.h"
#include "race.h"
#include "city_type.h"
#include "trade.h"

#include "window.h"
#include "cuss.h"
#include "glyph.h"    // For City::get_glyph()
#include <map>
#include <istream>    // For City::load_data()

// For determining City::get_glyph() - above this uses O, below uses o
#define CITY_POPULATION_LARGE 1000

class World_map;

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

class City
{
public:
  City();
  virtual ~City();

  virtual bool is_player_city() { return false; }
  virtual bool is_ai_city()     { return true;  }

  void clear_data();
  virtual std::string save_data();
  virtual bool load_data(std::istream& data);

// General-data mutators
  void set_world_map(World_map* W);
  virtual void start_new_city(World_map* W); // Sets up our population and more!
  void generate_map(Point loc = Point(-1, -1));
  void set_game(Game* G);
  virtual void do_turn();
  void set_random_name();
  void set_race(Race new_race);
  void set_city_type(City_type new_type);
  void setup_trade_routes();

  //void add_road_connection(City* neighbor);

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
  int uid;

  Point location;

  Citizens population[CIT_MAX];

  int resources[RES_MAX];
  int minerals[MINERAL_MAX];
  std::map<Animal,int> livestock;

  City_map map;

// Maps city UIDs to Trade_routes.
  std::map<int,Trade_route> trade_routes;

protected:
  std::string name;
  City_type type;
  Race race;
  Game* game;
  World_map* world;

// Which cities are we *directly* connected to via road?
  //std::vector<City*> road_connections;
};

#endif
