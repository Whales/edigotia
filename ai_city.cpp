#include "ai_city.h"
#include "window.h" // For debugmsg()
#include "world.h"  // Needed for randomize_properties()
#include "rng.h"
#include <sstream>

AI_mayor::AI_mayor()
{
  price_aggressiveness = 5;
}

AI_mayor::~AI_mayor()
{
}

void AI_mayor::randomize()
{
// Gives us a 20% chance of 5, 83% chance of [3, 7]
  price_aggressiveness = dice(2, 4) + rng(-1, 1);
}

AI_city::AI_city()
{
  type = CITY_TYPE_CITY;
  race = RACE_NULL;
  role = CITY_ROLE_NULL;
  radius = 1;
  for (int i = 0; i < CIT_MAX; i++) {
    population[i].type = Citizen_type(i);
    free_citizens[i] = 0;
  }
}

AI_city::~AI_city()
{
}

std::string AI_city::save_data()
{
  std::stringstream ret;
  ret << City::save_data() << std::endl;
  ret << int(role) << " ";
  ret << radius << " ";
  for (int i = 0; i < CIT_MAX; i++) {
    ret << free_citizens[i] << " ";
  }

  ret << areas_built.size() << " ";
  for (std::map<Area_type,int>::iterator it = areas_built.begin();
       it != areas_built.end();
       it++) {
    ret << int(it->first) << " " << it->second << " ";
  }
  ret << std::endl;

  ret << resource_production.size() << " ";
  for (std::map<Resource,int>::iterator it = resource_production.begin();
       it != resource_production.end();
       it++) {
    ret << int(it->first) << " " << it->second << " ";
  }
  ret << std::endl;

  ret << mineral_production.size() << " ";
  for (std::map<Mineral,int>::iterator it = mineral_production.begin();
       it != mineral_production.end();
       it++) {
    ret << int(it->first) << " " << it->second << " ";
  }

  return ret.str();
}

bool AI_city::load_data(std::istream& data)
{
  if (!City::load_data(data)) {
    debugmsg("AI_city::load_data() failed when calling City::load_data().");
    return false;
  }

  int tmprole;
  data >> tmprole;
  if (tmprole >= CITY_ROLE_MAX) {
    debugmsg("AI_city %s loaded City_role %d (range is 0 to %d).",
             name.c_str(), tmprole, CITY_ROLE_MAX - 1);
    return false;
  }

  role = City_role(tmprole);

  data >> radius;

  for (int i = 0; i < CIT_MAX; i++) {
    data >> free_citizens[i];
  }

  int num_areas;
  data >> num_areas;
  for (int i = 0; i < num_areas; i++) {
    int tmparea, tmpnum;
    data >> tmparea >> tmpnum;
    if (tmparea <= 0 || tmparea >= AREA_MAX) {
      debugmsg("AI_city %s loaded Area %d (range is 1 to %d).",
               name.c_str(), tmparea, AREA_MAX - 1);
      return false;
    }
    areas_built[ Area_type(tmparea) ] = tmpnum;
  }

  int num_resources;
  data >> num_resources;
  for (int i = 0; i < num_resources; i++) {
    int tmpres, tmpnum;
    data >> tmpres >> tmpnum;
    if (tmpres <= 0 || tmpres >= RES_MAX) {
      debugmsg("AI_city %s loaded resource %d (range is 1 to %d).",
               name.c_str(), tmpres, RES_MAX - 1);
      return false;
    }
    resource_production[ Resource(tmpres) ] = tmpnum;
  }

  int num_minerals;
  data >> num_minerals;
  for (int i = 0; i < num_minerals; i++) {
    int tmpmin, tmpnum;
    data >> tmpmin >> tmpnum;
    if (tmpmin <= 0 || tmpmin >= MINERAL_MAX) {
      debugmsg("AI_city loaded mineral %d (range is 1 to %d).",
               tmpmin, MINERAL_MAX - 1);
      return false;
    }
    mineral_production[ Mineral(tmpmin) ] = tmpnum;
  }

  return true;
}

void AI_city::randomize_properties(World_map* world)
{
  if (!world) {
    debugmsg("AI_city::randomize_properties(NULL) called!");
    return;
  }

// Go ahead and randomize our mayor
  mayor.randomize();

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

  population[CIT_PEASANT ].add_citizens(peasants );
  population[CIT_MERCHANT].add_citizens(merchants);
  population[CIT_BURGHER ].add_citizens(burghers );

  free_citizens[CIT_PEASANT ] = population[CIT_PEASANT ].count;
  free_citizens[CIT_MERCHANT] = population[CIT_MERCHANT].count;
  free_citizens[CIT_BURGHER ] = population[CIT_BURGHER ].count;

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

  if (radius > CITY_MAP_SIZE / 2) {
    radius = CITY_MAP_SIZE / 2;
  }
  
// Now, pick a City_role from our terrain.
  Map_type mtype = world->get_map_type(location);
  Map_type_datum* map_dat = Map_type_data[mtype];

// Pick a role.  Only accept roles that our race can handle.
  std::vector<City_role> available_roles;
  for (int i = 0; i < map_dat->city_roles.size(); i++) {
    City_role_datum* role_dat = City_role_data[ map_dat->city_roles[i] ];
    Race_skill skill = role_dat->skill;
    int skill_req = role_dat->skill_req;
    if (skill != SKILL_NULL || race_dat->skill_level[skill] >= skill_req) {
      available_roles.push_back( map_dat->city_roles[i] );
    }
  }
  if (available_roles.empty()) {
    role = CITY_ROLE_NULL;  // daaaaarn
  } else {
    int index = rng(0, available_roles.size() - 1);
    role = available_roles[index];
  }

// We ALWAYS get a keep!
  areas_built[AREA_KEEP] = 1;

  setup_resource_production(world);

  init_demands();
}

void AI_city::setup_resource_production(World_map* world)
{
  if (!world) {
    debugmsg("AI_city::setup_resource_production(NULL) called.");
    return;
  }

  resource_production.clear();
  mineral_production.clear();

// Data for our chosen role.
  City_role_datum* role_dat = City_role_data[role];

// A list of all tiles that are available for us to exploit.
  std::vector<Map_tile*> tiles;
  for (int x = 0 - radius; x <= radius; x++) {
    for (int y = 0 - radius; y <= radius; y++) {
      int mx = CITY_MAP_SIZE / 2 + x, my = CITY_MAP_SIZE / 2 + y;
      Map_tile* tmp_tile = map.get_tile(mx, my);
      if (!tmp_tile) {
        debugmsg("AI_city's map returned NULL for get_tile(%d, %d)!",
                 mx, my);
      } else {
        tiles.push_back( map.get_tile(mx, my) );
      }
    }
  }

// Figure out the food that we need...
  int food_req = get_food_consumption();
/* We multiply by 10,000 to avoid rounding errors.  10,000 is the product of:
 * 100 - terrain farmability is a percentage (0 to 100)
 * 100 - crop food output is per 100 units of the crop.
 * We also multiply by our chosen role's food_percentage, then divide by 100.
 * This is because some roles aim to produce more food than we require, while
 * others produce less and assume we'll import the deficit.
 */
  food_req = (food_req * 10000 * role_dat->food_percentage) / 100;
// TODO: If there's a nearby, friendly CITY_ROLE_FARMING city, decrease food_req


// Now try to match the required food.

// ** FARMING **
  int farm_skill = Race_data[race]->skill_level[SKILL_FARMING];
  if (farm_skill >= 2) {
    add_farms(tiles, food_req);
  } // if (farm_skill > 0)


  if (food_req > 0) { // Oh no, we still need food!
// We can get food from hunting or livestock.  Which are we better at?
    int hunting_skill   = Race_data[race]->skill_level[SKILL_HUNTING];
    int livestock_skill = Race_data[race]->skill_level[SKILL_LIVESTOCK];
    bool added_hunting_camps = false;

// Hunting is generally better than keeping livestock, so use it in a tie.
    if (hunting_skill >= 2 && hunting_skill >= livestock_skill) {
      added_hunting_camps = true;
      add_hunting_camps(tiles, food_req);
    }

// If we still need some food, add some livestock!
    if (food_req > 0 && livestock_skill >= 2) {
      add_pastures(tiles, food_req);
    }

// Finally, if we didn't add hunting camps on our first try, add them now.
    if (!added_hunting_camps && food_req > 0 && hunting_skill >= 2) {
      add_hunting_camps(tiles, food_req);
    }
  }

// Okay!  We are done with setting up food production.
// Now set up some production based on our City_role.
  switch (role) {
    case CITY_ROLE_NULL:
// We can add more buildings!
// TODO: Put this in a function and make it more good-er.
      areas_built[AREA_MARKETPLACE] = 3;
      break;
    case CITY_ROLE_FARMING:
      add_farms(tiles);
      break;
    case CITY_ROLE_HUNTING:
      add_hunting_camps(tiles);
      break;
    case CITY_ROLE_LIVESTOCK:
      add_pastures(tiles);
      break;
    case CITY_ROLE_MINING:
      add_mines(tiles);
      break;
    case CITY_ROLE_LOGGING:
      add_sawmills(tiles);
      break;
  } // switch (role)

// Finally, add some buildings to create more advanced resources.
  bool adding_buildings = (get_net_buildings_supported() > 0 &&
                           add_random_building());
  while (get_net_buildings_supported() > 0 && adding_buildings) {
    adding_buildings = add_random_building();
  }
  
}

int AI_city::get_net_food()
{
  return resource_production[RES_FOOD] - get_food_consumption();
}

int AI_city::get_daily_demand(Resource res)
{
  if (resource_demand.count(res)) {
    return resource_demand[res];
  }
  return 0;
}

int AI_city::get_daily_demand(Mineral min)
{
  if (mineral_demand.count(min)) {
    return mineral_demand[min];
  }
  return 0;
}

int AI_city::get_gross_resource_production(Resource res)
{
  if (resource_production.count(res)) {
    return resource_production[res];
  }
  return 0;
}

void AI_city::set_all_prices()
{
  for (int i = RES_NULL + 1; i < RES_MAX; i++) {
    Resource res = Resource(i);
    Resource_datum* res_dat = Resource_data[res];
    if (!res_dat->meta) {
      set_price(res);
    }
  }
}

std::string AI_city::list_production()
{
  std::stringstream ret;
  ret << "Type: " << City_role_data[role]->name << std::endl <<
         "Radius: " << radius << std::endl;
  ret << "Areas: ";
  if (areas_built.empty()) {
    ret << "<c=red>None<c=/>" << std::endl;
  } else {
    ret << std::endl;
    for (std::map<Area_type,int>::iterator it = areas_built.begin();
         it != areas_built.end();
         it++) {
      ret << "  " << Area_data[it->first]->name << " x " << it->second <<
             std::endl;
    }
  }

  for (std::map<Resource,int>::iterator it = resource_production.begin();
       it != resource_production.end();
       it++) {
    Resource res = it->first;
    int amount = it->second;
    ret << Resource_data[res]->name << ": " << amount << std::endl;
  }
  for (std::map<Mineral,int>::iterator it = mineral_production.begin();
       it != mineral_production.end();
       it++) {
    Mineral min = it->first;
    int amount = it->second;
    ret << Mineral_data[min]->name << ": " << amount << std::endl;
  }
  return ret.str();
}

int AI_city::get_net_buildings_supported()
{
  int ret = 0;
  for (std::map<Area_type,int>::iterator it = areas_built.begin();
       it != areas_built.end();
       it++) {
    ret += Area_data[it->first]->buildings_supported;
  }
  for (std::map<Building_type,int>::iterator it = buildings_built.begin();
       it != buildings_built.end();
       it++) {
    ret -= it->second;
  }

  return ret;
}

void AI_city::add_farms(std::vector<Map_tile*>& tiles, int& food_req)
{
  bool unlimited_food = (food_req == -1);
  int farm_skill = Race_data[race]->skill_level[SKILL_FARMING];
  Building_datum* farm_dat = Building_data[BUILD_FARM];
  int res_farming = farm_dat->amount_produced(RES_FARMING);
  int num_workers = farm_dat->get_total_jobs(CIT_PEASANT);
// Find the most food-producing tile.  OR, if we're not trying to produce food,
// find the most resource-value-producing tile.
  bool done = false;
  while (!done && !tiles.empty() && free_citizens[CIT_PEASANT] > 0 &&
         (unlimited_food || food_req > 0)) {
    int best_food = 0, best_res_value = 0, best_index = -1, best_res_index = -1;
    for (int i = 0; i < tiles.size(); i++) {
// Passing true to get_best_crop() prioritizes food output.
      Crop best_crop = tiles[i]->get_best_crop( !unlimited_food );
      Crop_datum* crop_dat = Crop_data[best_crop];
      int farmability = tiles[i]->get_farmability();
      int food = farmability * crop_dat->food;
      int res_value = 0;
      for (int n = 0; n < crop_dat->bonus_resources.size(); n++) {
        Resource_amount res_amt = crop_dat->bonus_resources[n];
        Resource_datum* res_dat = Resource_data[res_amt.type];
        res_value += res_dat->value * res_amt.amount;
      }
      if (unlimited_food) { // Check resource value first
        if (res_value > best_res_value) {
          best_res_value = res_value;
          best_index = i;
        } else if (food > best_food) {
          best_food = food;
          best_index = i;
        }
      } else {  // Check food output first
        if (food > best_food) {
          best_food = food;
          best_index = i;
        } else if (res_value > best_res_value) {
          best_res_value = res_value;
          best_res_index = i; // Used only if best_index == -1
        }
      }
    } // for (int i = 0; i < tiles.size(); i++)
    if (best_index == -1 && best_res_index == -1) { // No useful tiles!
      done = true;
    } else {
// If we couldn't find good food crops, use good resource crop instead.
      if (best_index == -1) {
        best_index = best_res_index;
      }

      Map_tile* farm_tile = tiles[best_index];
      int farmability = farm_tile->get_farmability();
      Crop crop = farm_tile->get_best_crop( !unlimited_food );
      Crop_datum* crop_dat = Crop_data[crop];
// Multiply by our race's farming skill, and res_farming from above.
      if (free_citizens[CIT_PEASANT] < num_workers) {
        num_workers = free_citizens[CIT_PEASANT];
      }
      int food_grown = crop_dat->food * farmability * farm_skill * res_farming *
                       num_workers;
      food_req -= food_grown;
      add_resource_production(RES_FOOD, food_grown / 10000);

// Add any resources, too
      for (int i = 0; i < crop_dat->bonus_resources.size(); i++) {
        Resource_amount res_amt = crop_dat->bonus_resources[i];
        res_amt.amount *= farmability * farm_skill * res_farming * num_workers;
        add_resource_production(res_amt.type, res_amt.amount / 10000);
      }
      free_citizens[CIT_PEASANT] -= num_workers;
      add_area(AREA_FARM);
      tiles.erase(tiles.begin() + best_index);
    }
  } // while (!done && food_req > 0 && !tiles.empty())
}

void AI_city::add_farms(std::vector<Map_tile*>& tiles)
{
  int temp = -1;
  add_farms(tiles, temp);
}

void AI_city::add_hunting_camps(std::vector<Map_tile*>& tiles, int& food_req)
{
  bool unlimited_food = (food_req == -1);
  int hunting_skill = Race_data[race]->skill_level[SKILL_HUNTING];
  Building_datum* camp_dat = Building_data[BUILD_HUNTING_CAMP];
  int res_hunting = camp_dat->amount_produced(RES_HUNTING);
  int num_workers = camp_dat->get_total_jobs(CIT_PEASANT);
// Find the most food-producing tile.
  bool done = false;
  while (!done && !tiles.empty() && free_citizens[CIT_PEASANT] > 0 &&
         (unlimited_food || food_req > 0)) {
    int best_food = 0, best_index = -1;
    for (int i = 0; i < tiles.size(); i++) {
      int food = tiles[i]->get_avg_hunting_output();
      if (food > best_food) {
        best_food = food;
        best_index = i;
      }
    }
    if (best_index == -1) { // No tiles produce food!  At all!
      done = true;
    } else {
// Multiply by our race's hunting skill, and res_hunting from above.
// Also multiply by 10,000 since food_req is multiplied by 10,000!  But also
// divide by 10 since we multiply by (5 + hunting_skill).
      if (free_citizens[CIT_PEASANT] < num_workers) {
        num_workers = free_citizens[CIT_PEASANT];
      }
      best_food = (1000 * best_food * (5 + hunting_skill) * res_hunting *
                   num_workers);
      food_req -= best_food;
      free_citizens[CIT_PEASANT] -= num_workers;
      add_resource_production(RES_FOOD, best_food / 10000);
      add_area(AREA_HUNTING_CAMP);
      tiles.erase(tiles.begin() + best_index);
    }
  } // while (!done && food_req > 0 && !tiles.empty())
}

void AI_city::add_hunting_camps(std::vector<Map_tile*>& tiles)
{
  int temp = -1;
  add_hunting_camps(tiles, temp);
}

void AI_city::add_pastures(std::vector<Map_tile*>& tiles, int& food_req)
{
  bool unlimited_food = (food_req == -1);
  int livestock_skill = Race_data[race]->skill_level[SKILL_LIVESTOCK];
/* First, figure out the three most food-producing animals.
 * We look at how much food is produced in 10,000 days; this makes calculating
 * the food we can get by slaughtering animals easier.  It also makes our food
 * amounts work directly with food_req (since that's multiplied by 10,000).
 * If food_req is -1, then we're not just trying to produce food; other
 * resources are valuable too.  So pick ANY livestock animals.
 */
  std::vector<Animal> food_animals;
  std::vector<int> food_amounts;
  int min_food = 0;
  for (int i = 0; i < ANIMAL_MAX; i++) {
    Animal_datum* ani_dat = Animal_data[i];
// Only multiply food_livestock by 100 since it's measured per 100 animals
    int animal_food = ani_dat->food_livestock * 100;
// Now, food from slaughtering animals (divided by how long it takes to birth 1)
// We multiply by 10,000 to match our required food; then also multiply by 100
// since we have 100 animals (and thus need to divide reproduction_rate by 100)
    animal_food += (1000000 * ani_dat->food_killed) /
                   ani_dat->reproduction_rate;
// At livestock skill of 1, tameness must be >= 88; at skill of 5, >= 40
    if (ani_dat->tameness + 12 * livestock_skill >= 100 &&
        (unlimited_food ||
         (animal_food >= min_food || food_animals.size() < 3))) {
// Insert it in the proper place.
      bool inserted = false;
      for (int n = 0; !inserted && n < food_animals.size(); n++) {
        if (food_amounts[n] < animal_food) {
          inserted = true;
          food_animals.insert( food_animals.begin() + n, Animal(i)   );
          food_amounts.insert( food_amounts.begin() + n, animal_food );
        }
      }
      if (!inserted) { // Didn't find a place to put it; push it to the end.
        food_animals.push_back( Animal(i)   );
        food_amounts.push_back( animal_food );
      }
    }
// Clip our vector to the proper size - unless we're using unlimited food
    if (!unlimited_food) {
      while (food_animals.size() > 3) {
        food_animals.pop_back();
        food_amounts.pop_back();
        min_food = food_amounts.back();
      }
    }
  }

// Since all tiles support pastures equally, we just randomly pick some from all
// tiles that support pastures.
  std::vector<Map_tile*> pastures;
  std::vector<int> pasture_indices; // For deleting them from tiles
  for (int i = 0; i < tiles.size(); i++) {
    if (tiles[i]->can_build(AREA_PASTURE)) {
      pastures.push_back( tiles[i] );
      pasture_indices.push_back( i );
    }
  }
  while (!pastures.empty() && (unlimited_food || food_req > 0)) {
    int index = rng(0, pastures.size() - 1);
    int orig_index = pasture_indices[index];
    pastures.erase( pastures.begin() + index );
    tiles.erase( tiles.begin() + orig_index );
// Pick an animal!
    int animal_index = rng(0, food_animals.size() - 1);
    Animal new_livestock = food_animals[ animal_index ];
    int food_amount = 100 * food_amounts[ animal_index ];
    food_req -= food_amount;
    add_resource_production(RES_FOOD, food_amount / 10000);
    add_area(AREA_PASTURE);

// Add resource production for anything else the animal may produce.
    Animal_datum* ani_dat = Animal_data[new_livestock];
    for (int i = 0; i < ani_dat->resources_livestock.size(); i++) {
      Resource_amount res_amt = ani_dat->resources_livestock[i];
      add_resource_production(res_amt);
    }
    for (int i = 0; i < ani_dat->resources_killed.size(); i++) {
      Resource_amount res_amt = ani_dat->resources_killed[i];
// Multiply by 100 since we have 100 animals birthing - effectively this
// divides reproduction_rate by 100.
      res_amt.amount = (res_amt.amount * 100) / ani_dat->reproduction_rate;
      if (res_amt.amount == 0) {
        res_amt.amount = 1;
      }
      add_resource_production(res_amt);
    }
    if (livestock.count(new_livestock)) {
      livestock[new_livestock] += 100;
    } else {
      livestock[new_livestock] = 100;
    }
  } // while (!pastures.empty() && food_req > 0)
}

void AI_city::add_pastures(std::vector<Map_tile*>& tiles)
{
  int temp = -1;
  add_pastures(tiles, temp);
}

void AI_city::add_mines(std::vector<Map_tile*>& tiles)
{
// TODO: Get info on how much a mine outputs per worker, and our race's skill
  Building_datum* mine_dat = Building_data[BUILD_MINE];
  int num_workers = mine_dat->get_total_jobs(CIT_PEASANT);
  for (int i = 0; free_citizens[CIT_PEASANT] > 0 && i < tiles.size(); i++) {
    Map_tile* tile = tiles[i];
    if (tile->can_build(AREA_MINE)) {
// Remove the tile from availability.
      tiles.erase(tiles.begin() + i);
      i--;
      if (free_citizens[CIT_PEASANT] < num_workers) {
        num_workers = free_citizens[CIT_PEASANT];
      }
      add_area(AREA_MINE);
      for (int n = 0; n < tile->minerals.size(); n++) {
        add_mineral_production(tile->minerals[n].type, num_workers);
      }
    }
  }
}

void AI_city::add_sawmills(std::vector<Map_tile*>& tiles)
{
// TODO: Get info on how much a sawmill outputs per worker, and our race's skill
  Building_datum* sawmill_dat = Building_data[BUILD_SAWMILL];
  int res_wood = sawmill_dat->amount_produced(RES_LOGGING);
  int num_workers = sawmill_dat->get_total_jobs(CIT_PEASANT);
  for (int i = 0; free_citizens[CIT_PEASANT] > 0 && i < tiles.size(); i++) {
    Map_tile* tile = tiles[i];
    if (tile->can_build(AREA_SAWMILL) && tile->wood >= 3000) {
// Remove the tile from availability.
      tiles.erase(tiles.begin() + i);
      i--;
      if (free_citizens[CIT_PEASANT] < num_workers) {
        num_workers = free_citizens[CIT_PEASANT];
      }
      add_resource_production(RES_WOOD, num_workers * res_wood);
      add_area(AREA_SAWMILL);
    }
  }
}

void AI_city::add_area(Area_type type)
{
  areas_built[type]++;
}

bool AI_city::add_random_building()
{
// This filters out any buildings that are just data for Areas.
  std::vector<Building_type> buildable = get_true_building_types();
  int start = rng(0, buildable.size() - 1);
  bool done = false;
  for (int i = start + 1; !done && i != start; i++) {
    if (i == buildable.size()) {
      i = 0; // Loop around to the start
      if (start == 0) {
        done = true;
      }
    }
    Building_type type = buildable[i];
    Building_datum* build_dat = Building_data[type];
    bool can_build = true;
// Check 1: Do we have available employees
    Citizen_amount jobs = build_dat->jobs;
    if (free_citizens[jobs.type] <= 0) {
      can_build = false;
    }
// Check 2: Ensure we can (and want to) pay the maintenance cost
    for (int n = 0; can_build && n < build_dat->maintenance_cost.size(); n++) {
      Resource_amount res_amt = build_dat->maintenance_cost[n];
      int deficit = resource_production[res_amt.type] - res_amt.amount;
// TODO: Don't hardcode this value.
      if (deficit < -5) {
        can_build = false;
      }
    }
// Check 3: Does it make something we want
    bool good_output = false;
    for (int n = 0; !good_output && n < build_dat->production.size(); n++) {
      Resource_amount res_amt = build_dat->production[n];
      if (resource_production.count(res_amt.type) == 0) {
// If it produces something we don't have, we want it!
        good_output = true;
      } else {
// Randomly decide if we want more of that resource.  If the building gives us
// five percent of our current output, it's an even split; the more we already
// produce the less likely it is that we want more.
        int bldg_roll = rng(0, 20 * res_amt.amount);
        int cur_roll  = rng(0, resource_production[res_amt.type]);
        if (bldg_roll >= cur_roll) {
          good_output = true;
        }
      }
    }

    if (!good_output && !build_dat->recipes.empty()) {
// If it doesn't have any constant production we want, maybe we want a recipe?
      for (int n = 0; !good_output && n < build_dat->recipes.size(); n++) {
        Recipe recipe = build_dat->recipes[n];
// Ensure that we have all the ingredients.
        bool has_ingredients = false;
        for (int m = 0;
             !has_ingredients && m < recipe.resource_ingredients.size();
             m++) {
          Resource_amount res_amt = recipe.resource_ingredients[m];
          int max_deficit = recipe.max_deficit;
          int deficit = resource_production[res_amt.type] - res_amt.amount;
          if (deficit > max_deficit) {
            has_ingredients = true;
          }
        }
        for (int m = 0;
             !has_ingredients && m < recipe.mineral_ingredients.size();
             m++) {
          Mineral_amount min_amt = recipe.mineral_ingredients[m];
          int max_deficit = recipe.max_deficit;
          int deficit = mineral_production[min_amt.type] - min_amt.amount;
          if (deficit > max_deficit) {
            has_ingredients = true;
          }
        }
// If we have the ingredients, verify that we actually want the output (same as
// above)
        if (has_ingredients) {
          Resource_amount res_amt = recipe.result;
          if (resource_production.count(res_amt.type) == 0) {
// If it produces something we don't have, we want it!
            good_output = true;
          } else {
// Randomly decide if we want more of that resource.  If the building gives us
// five percent of our current output, it's an even split; the more we already
// produce the less likely it is that we want more.
            int bldg_roll = rng(0, 20 * res_amt.amount);
            int cur_roll  = rng(0, resource_production[res_amt.type]);
            if (bldg_roll >= cur_roll) {
              good_output = true;
            }
          }
        }
      } // for (int n = 0; !good_output && n < build_dat->recipes.size(); n++)
    } // if (!good_output && !build_dat->recipes.empty())

    if (can_build && good_output) {
// We can build it, and we want to build it, so build it!
      if (add_building(type)) { // add_building() returns false on fail
        return true;
      }
    }
  } // for (int i = start_type + 1; i != start_type; i++)

// If we reach this point, we didn't find a single building we want!
  return false;
}

bool AI_city::add_building(Building_type type)
{
  Building_datum* build_dat = Building_data[type];
  int workers = build_dat->jobs.amount;
  if (free_citizens[build_dat->jobs.type] == 0) {
    return false;
  } else if (free_citizens[build_dat->jobs.type] < workers) {
    workers = free_citizens[build_dat->jobs.type];
  }

  free_citizens[build_dat->jobs.type] -= workers;
// Add the production.
  for (int i = 0; i < build_dat->production.size(); i++) {
    Resource_amount res_amt = build_dat->production[i];
    res_amt.amount *= workers;
    resource_production[res_amt.type] += res_amt.amount;
  }

  buildings_built[type]++;
  return true;
}

void AI_city::add_resource_production(Resource_amount res_amt)
{
  add_resource_production(res_amt.type, res_amt.amount);
}

void AI_city::add_resource_production(Resource res, int amount)
{
  if (resource_production.count(res)) {
    resource_production[res] += amount;
  } else {
    resource_production[res] = amount;
  }
}

void AI_city::add_mineral_production(Mineral_amount min_amt)
{
  add_mineral_production(min_amt.type, min_amt.amount);
}

void AI_city::add_mineral_production(Mineral min, int amount)
{
  if (mineral_production.count(min)) {
    mineral_production[min] += amount;
  } else {
    mineral_production[min] = amount;
  }
}

void AI_city::init_demands()
{
// At the bottom of this function, we reduce each demand value by the amount we
// produce.  We'll do food & gold after this reduction!

// Demand for gold is always infinite.  We just love the stuff!
  resource_demand[RES_GOLD] = INFINITE_RESOURCE;

// We always want to have a healthy production of wood and stone, to help us
// build more stuff!
  resource_demand[RES_WOOD]  = 25;
  resource_demand[RES_STONE] = 25;

// Go through all buildings and find anything we need for recipes.
  for (std::map<Building_type,int>::iterator it = buildings_built.begin();
       it != buildings_built.end();
       it++) {
    Building_type type = it->first;
    Building_datum* build_dat = Building_data[type];
// Here, "amount" refers to the amount of workers at this building type.
    int workers = it->second * build_dat->jobs.amount;

    for (int i = 0; i < build_dat->recipes.size(); i++) {
      Recipe rec = build_dat->recipes[i];
      for (int n = 0; n < rec.resource_ingredients.size(); n++) {
        Resource_amount res_amt = rec.resource_ingredients[n];
        res_amt.amount *= workers;
        resource_demand[res_amt.type] += res_amt.amount;
      }
      for (int n = 0; n < rec.mineral_ingredients.size(); n++) {
        Mineral_amount min_amt = rec.mineral_ingredients[n];
        min_amt.amount *= workers;
        mineral_demand[min_amt.type] += min_amt.amount;
      }
    }
  }

// We might also demand luxuries!  First, pick what we like.
  for (int i = CIT_PEASANT; i < CIT_BURGHER; i++) {
    population[i].pick_luxuries(this);
  }

// Now, set up demand for all luxuries.
  for (int i = 0; i < RES_MAX; i++) {
    Resource res = Resource(i);
    Resource_datum* res_dat = Resource_data[i];
    if (res_dat->morale > 0) {
      int total_demand = 0;
      for (int n = CIT_PEASANT; n < CIT_BURGHER; n++) {
        int demand = (res_dat->demand * population[n].count) / 100;
        Luxury_type lux_type = res_dat->luxury_type;
        if (lux_type != LUX_NULL &&
            population[n].luxury_demands[lux_type] != res) {
          demand *= 0.5;
        }
        total_demand += demand;
      }
      resource_demand[res] += total_demand;
    }
  }

// Now reduce all demands by the amount we produce.
  for (int i = 0; i < RES_MAX; i++) {
    Resource res = Resource(i);
    int produced = get_gross_resource_production(res);
    if (resource_demand.count(res) && resource_demand[res] > 0) {
      if (resource_demand[res] < produced) {
        resource_demand.erase(res);
      } else {
        resource_demand[res] -= produced;
      }
    }
  }

  for (int i = 0; i < MINERAL_MAX; i++) {
    Mineral min = Mineral(i);
    int produced = (mineral_production.count(min) ? mineral_production[min] :0);
    if (mineral_demand.count(min) && mineral_demand[min] > 0) {
      if (mineral_demand[min] < produced) {
        mineral_demand.erase(min);
      } else {
        mineral_demand[min] -= produced;
      }
    }
  }

// We always want gold!  We just love the stuff!
  resource_demand[RES_GOLD] = INFINITE_RESOURCE;

// Food demand is equivalent to 110% of our food deficit.
  int net_food = get_net_food();
  if (net_food < 0) {
    resource_demand[RES_FOOD] = 1.1 * (0 - net_food);
  }

}

void AI_city::set_price(Resource res)
{
  if (res == RES_NULL) {
    debugmsg("AI_city::set_price(RES_NULL) called!");
    return;
  }
  if (res == RES_MAX) {
    debugmsg("AI_city::set_price(RES_MAX) called!");
    return;
  }

  Resource_datum* res_dat = Resource_data[res];
  if (res_dat->meta) {
    debugmsg("AI_city::set_price(%s) called (it's a meta-resource)!.",
             res_dat->name.c_str());
    return;
  }

  resource_price[res] = res_dat->value;

  std::vector<Trade_route> buyers = find_buyers_for(res);

  if (buyers.empty()) {
    return; // No one will buy it (for now), so just charge the "normal" price!
  }

  int avg_overhead;
  for (int i = 0; i < buyers.size(); i++) {
    avg_overhead += buyers[i].overhead;
  }
  avg_overhead /= buyers.size();

/* So, our price reduction formula looks like:
 * ( (Normal Price) * (Avg Overhead) ) / (Price Aggressiveness)
 * AI_mayor's price_aggressiveness value is on a scale from 1 to 9, but we want
 * it to be on a scale of [25 * (3 to 11)]; also, in AI_mayor a higher number is
 * more aggressive, but here lower is more aggressive.  So our actual
 * aggressiveness is 25 * (12 - mayor.price_aggressiveness).
 */

  int aggressiveness = 25 * (12 - mayor.price_aggressiveness);
  int reduction = (res_dat->value * avg_overhead) / aggressiveness;
  resource_price[res] -= reduction;
}
