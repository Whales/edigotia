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
  virtual void start_new_city(); // Sets up our population and more!
  void generate_map(Point loc = Point(-1, -1));
  virtual void do_turn();
  void set_random_name();
  void set_race(Race new_race);
  void set_city_type(City_type new_type);
  void setup_trade_routes(int range = 100, bool progress_bar = true);

  //void add_road_connection(City* neighbor);

// General-data accessors
  int get_total_population(Citizen_type type = CIT_NULL);
  std::string get_name();
  glyph get_glyph();
  City_type get_city_type();
  Race get_race();

// Trade-related functions
  std::vector<Trade_route> find_sellers_of(Resource res);
/* find_buyers_for() looks at all our Trade_routes (the potential cities we can
 * trade with).  For each of those target cities, if the target wants the
 * resource in question, it runs find_sellers_of(res).  If we are the FIRST city
 * in that list, then that target city is returned by find_buyers_for(res).  If
 * we are not the first city, then the target city will buy from one of our
 * competitors instead!
 */
  std::vector<Trade_route> find_buyers_for(Resource res);
  int get_price(Resource res);
  int get_price(Mineral  min);

// Set prices for all resources.
  virtual void set_all_prices();

// The quantity that we want on a daily basis.
// NOTE: Both of these are overridden for AI_city and Player_city; as such, the
// City version just returns 0.
  virtual int get_daily_demand(Resource res);
  virtual int get_daily_demand(Mineral  min);

// Resource-related functions

// Returns all resources we own with the given Luxury_type.
  std::map<Resource,int> get_luxuries(Luxury_type type);

// The following functions do NOT include slightly-random sources of resources,
// such as the resources produced by livestock when they die

// Total amount produced daily, including from buildings and imports
// Note that this generic City version just returns 0, since we don't have
// Areas and Buildings, even fake ones.
  virtual int get_gross_resource_production(Resource res);
// Total amount expended daily, including use by citizens and exports
  virtual int get_resource_consumption     (Resource res);
// Gross production minus gross consumption
  virtual int get_net_resource_production  (Resource res);

  int get_resource_amount(Resource res);
  int get_mineral_amount (Mineral  min);
  int get_food_cap();         // Returns get_food_consumption() * 10 (days)
  int get_food_consumption(Citizen_type type = CIT_NULL); // Per day

  bool has_resource (Resource res, int amount = 1);
  bool has_resource (Resource_amount res);
  bool has_resources(std::vector<Resource_amount> res_used);
  bool has_resources(std::map<Resource,int>       res_used);

  bool has_mineral (Mineral res, int amount = 1);
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

  void gain_mineral (Mineral res, int amount);
  void gain_mineral (Mineral_amount res);
  void gain_minerals(std::vector<Mineral_amount> res_used);
  void gain_minerals(std::map<Mineral,int>       res_used);


// Public data
  int uid;

  Point location;

  Citizens population[CIT_MAX];

  int resources[RES_MAX];
  int minerals[MINERAL_MAX];
  std::map<Animal,int> livestock;

  City_map map;

// Maps city UIDs to Trade_routes.
// These aren't *active* trade routes, just potential ones!
  std::map<int,Trade_route> trade_routes;

protected:
  std::string name;
  City_type type;
  Race race;

  int resource_price[RES_MAX];
  int mineral_price[MINERAL_MAX];

// Which cities are we *directly* connected to via road?
  //std::vector<City*> road_connections;
};

#endif
