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

private:
  City_role role;

/* City control radius, like the player uses, except we only use it to figure
 * out what tiles we're going to be exploiting.  It's also largely abstracted;
 * based on our population.
 */
  int radius;
  std::map<Resource,int> resource_production;

};

#endif
