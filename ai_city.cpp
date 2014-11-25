#include "ai_city.h"
#include "window.h" // For debugmsg()
#include "world.h"  // Needed for randomize_properties()
#include "rng.h"

AI_city::AI_city()
{
  type = CITY_TYPE_CITY;
  race = RACE_NULL;
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
  int peasants = pop - merchants - burghers;

  population[CIT_PEASANT ].add_citizens(peasants);
  population[CIT_MERCHANT].add_citizens(merchants);
  population[CIT_BURGHER ].add_citizens(burghers);

// Now, pick a City_role from our terrain.
  Map_type mtype = world->get_map_type(location);
  Map_type_datum* map_dat = Map_type_data[mtype];

  if (map_dat->city_roles.empty()) {
    role = CITY_ROLE_NULL;  // daaaaarn
  } else {
    int index = rng(0, map_dat->city_roles.size() - 1);
    role = map_dat->city_roles[index];
  }
}
