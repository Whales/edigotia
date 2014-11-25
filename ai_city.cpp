#include "ai_city.h"
#include "window.h" // For debugmsg()
#include "world.h"  // Needed for randomize_properties()
#include "rng.h"

AI_city::AI_city()
{
  type = CITY_TYPE_CITY;
  race = RACE_NULL;
  role = CITY_ROLE_NULL;
  radius = 1;
  for (int i = 0; i < CIT_MAX; i++) {
    population[i].type = Citizen_type(i);
  }
}

AI_city::~AI_city()
{
}

void AI_city::randomize_properties(World_map* world)
{
  if (!world) {
    debugmsg("AI_city::randomize_properties(NULL) called!");
    return;
  }

  Race_datum* race_dat = Race_data[race];

// Set up our populations
  for (int i = 0; i < CIT_MAX; i++) {
    population[i].reset();
  }

  int pop = rng(race_dat->city_size_min[type], race_dat->city_size_max[type]);

// Figure out how to arrange pop, using the ratios from our race
/* Quick explanation of the equations below:
 * Let X = peasant pop, Y = merchant pop, Z = burgher pop, N = total pop
 * Let a = peasant:merchant ratio, b = merchant:burgher ratio.
 * Thus X = aY, Y = bZ, and N = X + Y + Z
 * Z = (N - Z) / (a * b)                    ===>   Z = N / (a * b + 1)
 * Y = X / a   ===>   Y = (N - Y - Z) / a   ===>   Y = (N - Z) / (a + 1)
 * X = N - Y - Z
 */

  int ratio_a = race_dat->citizen_ratio[CIT_MERCHANT],
      ratio_b = race_dat->citizen_ratio[CIT_BURGHER];
  if (ratio_a == 0 || ratio_b == 0) {
    debugmsg("Bad citizen_ratio in %s data!", race_dat->name.c_str());
    return;
  }
  int burghers  = pop / (1 + ratio_a * ratio_b);
  int merchants = (pop - burghers) / (1 + ratio_a);
  int peasants  = pop - merchants - burghers;

  population[CIT_PEASANT ].add_citizens(peasants);
  population[CIT_MERCHANT].add_citizens(merchants);
  population[CIT_BURGHER ].add_citizens(burghers);

// Figure out our radius, based on population.
  if (pop >= 10000) {
    radius = 3;
  } else if (pop >= 1000) {
    radius = 2;
  } else {
    radius = 1;
  }
  if (burghers >= 10) {
    radius += 2;
  } else if (burghers > 0) {
    radius += 1;
  }
  
// Now, pick a City_role from our terrain.
  Map_type mtype = world->get_map_type(location);
  Map_type_datum* map_dat = Map_type_data[mtype];

  if (map_dat->city_roles.empty()) {
    role = CITY_ROLE_NULL;  // daaaaarn
  } else {
    int index = rng(0, map_dat->city_roles.size() - 1);
    role = map_dat->city_roles[index];
  }


  setup_resource_production(world);
}

void AI_city::setup_resource_production(World_map* world)
{
  if (!world) {
    debugmsg("AI_city::setup_resource_production(NULL) called.");
    return;
  }

  resource_production.clear();

// A list of all tiles that are available for us to exploit.
  std::vector<Map_tile*> tiles;
  for (int x = 0 - radius; x <= radius; x++) {
    for (int y = 0 - radius; y <= radius; y++) {
      int mx = CITY_MAP_SIZE / 2 + x, my = CITY_MAP_SIZE / 2 + y;
      tiles.push_back( map.get_tile(mx, my) );
    }
  }

// Figure out the food that we need...
  int food_req = get_food_consumption();
// ...and try to match that need.
// TODO: Skip this is there's a nearby, friendly CITY_ROLE_FARMING city
//       (or at least include it in our considerations)
/*
  while (food_req > 0 && !tiles.empty()) {
// Find the most food-producing tile.
  }
*/
    
}


