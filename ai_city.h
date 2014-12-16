#ifndef _AI_CITY_H_
#define _AI_CITY_H_

#include "citizen.h"
#include "city.h"
#include "glyph.h"  // For AI_city::get_glyph()
#include "ai.h"     // For City_role

class World_map;

class AI_city : public City
{
public:
  AI_city();
  virtual ~AI_city();

  virtual bool is_ai_city() { return true; }

// Randomize population, role, and more
  void randomize_properties(World_map* world);
  void setup_resource_production(World_map* world);

// class City provides get_food_consumption()
// get_food_production() is just resource_production[RES_FOOD]
  int get_net_food(); // production - consumption, obvs

  std::string list_production();  // For now, this is just debug output.

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

  void add_resource_production(Resource_amount res_amt);
  void add_resource_production(Resource res, int amount);

  void add_mineral_production(Mineral_amount min_amt);
  void add_mineral_production(Mineral min, int amount);

  City_role role;

/* City control radius, like the player uses, except we only use it to figure
 * out what tiles we're going to be exploiting.  It's also largely abstracted;
 * based on our population.
 */
  int radius;
  int free_peasants;
  std::map<Resource,int> resource_production;
  std::map<Mineral,int>  mineral_production;

};

#endif
