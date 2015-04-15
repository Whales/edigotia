#ifndef _AI_CITY_H_
#define _AI_CITY_H_

#include "citizen.h"
#include "city.h"
#include "glyph.h"  // For AI_city::get_glyph()
#include "ai.h"     // For City_role
#include <string>
#include <istream>

class World_map;

// Parameters for decision-making
struct AI_mayor
{
  AI_mayor();
  ~AI_mayor();

  void randomize();

// *** DATA ***

// How aggressively do we reduce our export prices to combat high overhead?
// Range is 1 - 9; 9 may mean very low prices!
  int price_aggressiveness;
};

class AI_city : public City
{
public:
  AI_city();
  virtual ~AI_city();

  virtual bool is_ai_city() { return true; }

  virtual std::string save_data();
  virtual bool load_data(std::istream& data);

// Randomize population, role, and more
  void randomize_properties(World_map* world);
  void setup_resource_production(World_map* world);

// class City provides get_food_consumption()
// get_food_production() is just resource_production[RES_FOOD]
  int get_net_food(); // production - consumption, obvs

  virtual int get_daily_demand(Resource res);
  virtual int get_daily_demand(Mineral  min);

  virtual int get_gross_resource_production (Resource res);

  virtual void set_all_prices();

  std::string list_production();  // For now, this is just debug output.

// buildings_supported of all areas, minus existing buildings.
  int get_net_buildings_supported();

private:
// add_farms(), add_hunting_camps(), and add_pastures() have two forms; one with
// an int& and one without.  Use the one WITH the int& if you want to stop
// placing the areas once we reach that amount of food (50,000 times the amount
// we actually want, to avoid rounding errors).  It will modify the int& passed
// to reflect the amount of food gained.
// Use the form WITHOUT the int& if you want to keep placing areas until we're
// no longer able to (thus producing as much food as possible).
  void add_farms        (std::vector<Map_tile*>& tiles, int& food_req);
  void add_hunting_camps(std::vector<Map_tile*>& tiles, int& food_req);
  void add_pastures     (std::vector<Map_tile*>& tiles, int& food_req);
  void add_farms        (std::vector<Map_tile*>& tiles);
  void add_hunting_camps(std::vector<Map_tile*>& tiles);
  void add_pastures     (std::vector<Map_tile*>& tiles);
// These ones don't involve food, so they just need our list of tiles.
  void add_mines        (std::vector<Map_tile*>& tiles);
  void add_sawmills     (std::vector<Map_tile*>& tiles);

// This adds 1 to the count in areas_built
  void add_area(Area_type type);

/* add_random_building() randomly picks a starting point in Building_type, and
 * progresses forward from there (looping back to BUILD_NULL if necessary) until
 * it finds something we want to & can build.  It returns false if it doesn't
 * find anything; otherwise, it calls add_building() and returns true.
 */
  bool add_random_building();
  bool add_building(Building_type type);  // Returns true if we really built it

  void add_resource_production(Resource_amount res_amt);
  void add_resource_production(Resource res, int amount);

  void add_mineral_production(Mineral_amount min_amt);
  void add_mineral_production(Mineral min, int amount);

  void init_demands();  // Looks at what we lack for recipes etc
  void set_price(Resource res);

// *** DATA ***

  City_role role;

  AI_mayor mayor;

/* City control radius, like the player uses, except we only use it to figure
 * out what tiles we're going to be exploiting.  It's also largely abstracted;
 * based on our population.
 */
  int radius;
  int free_citizens[CIT_MAX]; // Free as in unemployed & otherwise not used

  std::map<Area_type,int>     areas_built;
  std::map<Building_type,int> buildings_built;

  std::map<Resource,int> resource_production;
  std::map<Mineral,int>  mineral_production;

  std::map<Resource,int> resource_demand;
  std::map<Mineral,int>  mineral_demand;

};

#endif
