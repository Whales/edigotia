#include "city.h"
#include "rng.h"
#include "world.h"
#include "globals.h"
#include <sstream>

City::City()
{
  uid = -1;
  type = CITY_TYPE_CITY;
  race = RACE_NULL;
  for (int i = 0; i < CIT_MAX; i++) {
    population[i].type = Citizen_type(i);
  }
  for (int i = 0; i < RES_MAX; i++) {
    resources[i] = 0;
    resource_price[i] = Resource_data[i]->value;
  }
  for (int i = 0; i < MINERAL_MAX; i++) {
    minerals[i] = 0;
    mineral_price[i] = Mineral_data[i]->value;
  }
}

City::~City()
{
}

void City::clear_data()
{
  uid = -1;
  name = "";
  type = CITY_TYPE_CITY;
  race = RACE_NULL;
  location = Point(-1, -1);
  for (int i = 0; i < CIT_MAX; i++) {
    population[i].type = Citizen_type(i);
  }
  for (int i = 0; i < RES_MAX; i++) {
    resources[i] = 0;
  }
  for (int i = 0; i < MINERAL_MAX; i++) {
    minerals[i] = 0;
  }
  livestock.clear();
  trade_routes.clear();
// TODO: Clear the map?
}

std::string City::save_data()
{
  std::stringstream ret;
  ret << uid << " ";
// Since some names are multiple words, we need the ! to mark the end.
  ret << name << " ! ";
  ret << int(type) << " " << int(race) << std::endl;
  ret << location.x << " " << location.y << std::endl;
  ret << map.save_data() << std::endl;
  ret << trade_routes.size() << std::endl;
  for (std::map<int,Trade_route>::iterator it = trade_routes.begin();
       it != trade_routes.end();
       it++) {
    ret << it->first << " " << it->second.save_data() << std::endl;
  }

  for (int i = 0; i < CIT_MAX; i++) {
    ret << population[i].save_data() << std::endl;
  }
  for (int i = 0; i < RES_MAX; i++) {
    ret << resources[i] << " ";
  }
  ret << std::endl;
  for (int i = 0; i < MINERAL_MAX; i++) {
    ret << minerals[i] << " ";
  }
  ret << std::endl;
  ret << livestock.size() << " ";
  for (std::map<Animal,int>::iterator it = livestock.begin();
       it != livestock.end();
       it++) {
    ret << int(it->first) << " " << it->second << " ";
  }
  ret << std::endl;
  return ret.str();
}

bool City::load_data(std::istream& data)
{
  clear_data();

  data >> uid;
  std::string tmpstr;
  while (tmpstr != "!") {
    data >> tmpstr;
    if (tmpstr != "!") {
      if (!name.empty()) {
        name = name + " ";
      }
      name = name + tmpstr;
    }
  }
  int tmptype, tmprace;
  data >> tmptype >> tmprace;
  if (tmptype >= CITY_TYPE_MAX || tmptype <= 0) {
    debugmsg("City '%s' has type value of %d (range is 1 - %d)!",
             name.c_str(), tmptype, CITY_TYPE_MAX - 1);
    return false;
  }
  if (tmprace >= RACE_MAX || tmptype <= 0) {
    debugmsg("City '%s' has race value of %d (range is 1 - %d)!",
             name.c_str(), tmprace, RACE_MAX - 1);
    return false;
  }
  type = City_type(tmptype);
  race = Race(tmprace);

  data >> location.x >> location.y;

  if (!map.load_data(data)) {
    debugmsg("City '%s' failed to load map.", name.c_str());
    return false;
  }

  int num_routes;
  data >> num_routes;
  for (int i = 0; i < num_routes; i++) {
    int tmpuid;
    Trade_route tmproute;
    data >> tmpuid;
    if (!tmproute.load_data(data)) {
      debugmsg("City '%s' failed to load trade route to City %d.",
               name.c_str(), tmpuid);
      return false;
    }
    trade_routes[tmpuid] = tmproute;
  }

  for (int i = 0; i < CIT_MAX; i++) {
    if (!population[i].load_data(data)) {
      debugmsg("City '%s' failed to load %s data.",
               name.c_str(), citizen_type_name(Citizen_type(i), true).c_str());
      return false;
    }
  }

  for (int i = 0; i < RES_MAX; i++) {
    data >> resources[i];
  }

  for (int i = 0; i < MINERAL_MAX; i++) {
    data >> minerals[i];
  }

  int num_animals;
  data >> num_animals;
  for (int i = 0; i < num_animals; i++) {
    int tmpanimal, tmpnum;
    data >> tmpanimal >> tmpnum;
    if (tmpanimal <= 0 || tmpanimal >= ANIMAL_MAX) {
      debugmsg("City '%s' had animal of index %d (Range is 1 to %d)",
               name.c_str(), tmpanimal, ANIMAL_MAX - 1);
      return false;
    }
    livestock[ Animal(tmpanimal) ] = tmpnum;
  }

  return true;
}

void City::start_new_city()
{
  Race_datum* race_dat = Race_data[race];
  type = CITY_TYPE_CITY;

  for (int i = 0; i < CIT_MAX; i++) {
    population[i].type = Citizen_type(i);
    population[i].add_citizens( race_dat->starting_population[i] );
  }

  for (int i = 0; i < RES_MAX; i++) {
    resources[i] = race_dat->starting_resources[i];
  }
  for (int i = 0; i < MINERAL_MAX; i++) {
    minerals[i] = 0;
  }

// We always start with the maximum amount of food, regardless of our race.
  resources[RES_FOOD]  = get_food_cap();

}

// loc defaults to (-1, -1)
void City::generate_map(Point loc)
{
  if (loc.x >= 0 && loc.x < WORLD_MAP_SIZE &&
      loc.y >= 0 && loc.y < WORLD_MAP_SIZE   ) {
    location = loc;
  }

  std::vector<Crop>    crops    = GAME->world->crops_at   (location);
  std::vector<Mineral> minerals = GAME->world->minerals_at(location);
  std::vector<Animal>  animals  = GAME->world->animals_at (location);

  map.generate( GAME->world->get_map_type(location),
                crops, minerals, animals,
                GAME->world->coast_from(location),
                GAME->world->river_start_for(location),
                GAME->world->river_end_for(location)
              );
}

void City::do_turn()
{
// TODO: Stick some stuff in here?
}

void City::set_random_name()
{
  if (race == RACE_NULL) {
    name = "Abandoned City";
  } else {
    Race_datum* race_dat = Race_data[race];
    name = race_dat->get_city_name();
  }
}

void City::set_race(Race new_race)
{
  race = new_race;
}

void City::set_city_type(City_type new_type)
{
  type = new_type;
}

// range defaults to 100, progress_bar defaults to true
void City::setup_trade_routes(int range, bool progress_bar)
{
  trade_routes.clear();

  for (int i = 0; i < GAME->world->city_list.size(); i++) {
    int percent = (100 * i) / GAME->world->city_list.size();
    if (progress_bar) {
      popup_nowait("Establishing trade routes... [%d%%%%%%%%]", percent);
    }
    City* target = GAME->world->city_list[i];
// Don't consider cities that are more than <range> tiles away.
    if (target != this && manhattan_dist(location, target->location) <= range) {
      int dist = GAME->world->get_trade_distance(race, location,
                                                 target->location);
      int overhd = dist + 5;
      if (target->race != race) {
        Race_datum* our_race_dat = Race_data[race];
        Race_datum* their_race_dat = Race_data[target->race];
        int our_relation = our_race_dat->relations[target->race];
        int their_relation = their_race_dat->relations[race];
// Positive relations have a smaller/no impact on this penalty.
        if (our_relation > 0) {
          our_relation /= 2;
        }
        if (their_relation > 0) {
          their_relation /= 2;
        }
        int penalty = 11 - our_relation - their_relation;
// If our races like each other, there might be no penalty!
        if (penalty > 10) {
          overhd = (overhd * penalty) / 10;
        }
      }
      if (dist >= 0 && dist <= 5000) { // 50 days' travel
        trade_routes[target->uid] = Trade_route(target->location, dist, overhd);
      }
    }
  }
}

int City::get_total_population(Citizen_type type)
{
  if (type == CIT_NULL) {
    int ret = 0;
    for (int i = 0; i < CIT_MAX; i++) {
      ret += population[i].count;
    }
    return ret;
  }
  return population[type].count;
}

std::string City::get_name()
{
  return name;
}

glyph City::get_glyph()
{
  glyph ret;
  ret.bg = c_black;
  int pop = get_total_population();
  switch (type) {
    case CITY_TYPE_CAPITAL:
      ret.symbol = '@';
      break;
    case CITY_TYPE_DUCHY:
      ret.symbol = 'D';
      break;
    case CITY_TYPE_CITY:
    default:
      if (pop >= CITY_POPULATION_LARGE) {
        ret.symbol = 'O';
      } else {
        ret.symbol = 'o';
      }
      break;
  }

// Now color us based on our population.
  int pop_level = 0;
  if (pop >= CITY_POPULATION_LARGE) {
// 100 if we're barely large, 200 if we're double-large, etc
    pop_level = (100 * pop) / CITY_POPULATION_LARGE;
  } else {
// 100 if we're half-large, 200 if we're nearly large, 0 if we're 0
    pop_level = (200 * pop) / CITY_POPULATION_LARGE;
  }

  if (pop_level >= 200) {
    ret.fg = c_white;
  } else if (pop_level >= 175) {
    ret.fg = c_ltred;
  } else if (pop_level >= 150) {
    ret.fg = c_yellow;
  } else if (pop_level >= 125) {
    ret.fg = c_ltgreen;
  } else if (pop_level >= 100) {
    ret.fg = c_ltblue;
  } else if (pop_level >= 75) {
    ret.fg = c_cyan;
  } else if (pop_level >= 50) {
    ret.fg = c_magenta;
  } else if (pop_level >= 25) {
    ret.fg = c_brown;
  } else {
    ret.fg = c_ltgray;
  }

  return ret;
}

City_type City::get_city_type()
{
  return type;
}

Race City::get_race()
{
  return race;
}

std::vector<Trade_route> City::find_sellers_of(Resource res)
{
  std::vector<Trade_route> ret;
  for (std::map<int,Trade_route>::iterator it = trade_routes.begin();
       it != trade_routes.end();
       it++) {
    City* seller = GAME->world->lookup_city_uid(it->first);
    if (seller) {
      int avail = seller->get_net_resource_production(res);
      if (avail > 0) {
// Insert into our return vector, sorted by distance.
// TODO: Sort by unit price (including overhead)!
        int dist = it->second.distance;
        bool found = false;
        for (int i = 0; !found && i < ret.size(); i++) {
          if (dist <= ret[i].distance) {
            found = true;
            ret.insert( ret.begin() + i, it->second );
          }
        }
        if (!found) {
          ret.push_back(it->second);
        }
      } // if (avail > 0)
    } // if (seller)
  } // for (std::map<int,Trade_route>::iterator it = trade_routes.begin(); ... )
  return ret;
}

std::vector<Trade_route> City::find_buyers_for(Resource res)
{
  std::vector<Trade_route> ret;
  for (std::map<int,Trade_route>::iterator it = trade_routes.begin();
       it != trade_routes.end();
       it++) {
    City* buyer = GAME->world->lookup_city_uid(it->first);
    if (buyer && buyer->get_daily_demand(res) > 0) {
      ret.push_back(it->second);
    }
  }
  return ret;
}

int City::get_price(Resource res)
{
  return resource_price[res];
}

int City::get_price(Mineral min)
{
  return mineral_price[min];
}

void City::set_all_prices()
{
}

// get_daily_demand() (both versions) are overridden for AI_city and Player_city
// and as such, the City version just returns 0 (it's not used).
int City::get_daily_demand(Resource res)
{
  return 0;
}

int City::get_daily_demand(Mineral min)
{
  return 0;
}

std::map<Resource,int> City::get_luxuries(Luxury_type type)
{
  std::map<Resource,int> ret;

  for (int i = 0; i < RES_MAX; i++) {
    if (resources[i] > 1 && Resource_data[i]->luxury_type == type) {
      ret[ Resource(i) ] = resources[i];
    }
  }

  return ret;
}

// Since generic City doesn't have any Areas or Buildings or imports, we don't
// actually produce resources.  Overloaded for Player_city and AI_city.
int City::get_gross_resource_production(Resource res)
{
  return 0;
}

int City::get_resource_consumption(Resource res)
{
  int ret = 0;
  for (int i = CIT_PEASANT; i <= CIT_BURGHER; i++) {
    ret += population[i].consumption[res];
    if (res == RES_FOOD) {  // Special case
      ret += get_food_consumption( Citizen_type(i) );
    }
  }
  return ret;
}

int City::get_net_resource_production(Resource res)
{
  return get_gross_resource_production(res) - get_resource_consumption(res);
}

int City::get_resource_amount(Resource res)
{
  return resources[res];
}

int City::get_mineral_amount(Mineral min)
{
  return minerals[min];
}

int City::get_food_cap()
{
  int food_expiration = 25; // How many days does it take our food to go bad?
// TODO: Allow modifiers to food_expiration.
  return food_expiration * get_food_consumption();
}

// type defaults to CIT_NULL
int City::get_food_consumption(Citizen_type type)
{
// TODO: Modify this based on our race.
  if (type == CIT_NULL) {
    int ret = 0;
    for (int i = 1; i < CIT_MAX; i++) {
      int a = get_food_consumption( Citizen_type(i) );
      ret += a;
    }
    return ret;
  }
// citizen_food_consumption() is defined in citizen.cpp.  It returns the amount
// per 100 citizens, so naturally we need to divide by 100.
  int ret = (population[type].count * citizen_food_consumption( type )) / 100;
// Modify based on race - its food_consumption is a percentage.
  ret = (ret * Race_data[race]->food_consumption) / 100;
  return ret;
}

// amount defaults to 1
bool City::has_resource(Resource res, int amount)
{
  return (get_resource_amount(res) >= amount);
}

bool City::has_resource(Resource_amount res)
{
  return has_resource(res.type, res.amount);
}

bool City::has_resources(std::vector<Resource_amount> res_used)
{
  for (int i = 0; i < res_used.size(); i++) {
    if (!has_resource( res_used[i] )) {
      return false;
    }
  }
  return true;
}

bool City::has_resources(std::map<Resource,int> res_used)
{
  for (std::map<Resource,int>::iterator it = res_used.begin();
       it != res_used.end();
       it++) {
    if (!has_resource(it->first, it->second)) {
      return false;
    }
  }
  return true;
}

// amount defaults to 1
bool City::has_mineral(Mineral res, int amount)
{
  return (get_mineral_amount(res) >= amount);
}

bool City::has_mineral(Mineral_amount res)
{
  return has_mineral(res.type, res.amount);
}

bool City::has_minerals(std::vector<Mineral_amount> min_used)
{
  for (int i = 0; i < min_used.size(); i++) {
    if (!has_mineral( min_used[i] )) {
      return false;
    }
  }
  return true;
}

bool City::has_minerals(std::map<Mineral,int> min_used)
{
  for (std::map<Mineral,int>::iterator it = min_used.begin();
       it != min_used.end();
       it++) {
    if (!has_mineral(it->first, it->second)) {
      return false;
    }
  }
  return true;
}

bool City::expend_resource(Resource res, int amount)
{
  if (!has_resource(res, amount)) {
    return false;
  }
  resources[res] -= amount;
  return true;
}

bool City::expend_resource(Resource_amount res)
{
  if (!has_resource(res)) {
    return false;
  }
  resources[res.type] -= res.amount;
  return true;
}

bool City::expend_resources(std::vector<Resource_amount> res_used)
{
  if (!has_resources(res_used)) {
    return false;
  }

  for (int i = 0; i < res_used.size(); i++) {
    resources[ res_used[i].type ] -= res_used[i].amount;
  }
  return true;
}

bool City::expend_resources(std::map<Resource,int> res_used)
{
  if (!has_resources(res_used)) {
    return false;
  }

  for (std::map<Resource,int>::iterator it = res_used.begin();
       it != res_used.end();
       it++) {
    resources[it->first] -= it->second;
  }
  return true;
}

bool City::expend_mineral(Mineral res, int amount)
{
  if (!has_mineral(res, amount)) {
    return false;
  }
  minerals[res] -= amount;
  return true;
}

bool City::expend_mineral(Mineral_amount res)
{
  if (!has_mineral(res)) {
    return false;
  }
  minerals[res.type] -= res.amount;
  return true;
}

bool City::expend_minerals(std::vector<Mineral_amount> min_used)
{
  if (!has_minerals(min_used)) {
    return false;
  }

  for (int i = 0; i < min_used.size(); i++) {
    minerals[ min_used[i].type ] -= min_used[i].amount;
  }
  return true;
}

bool City::expend_minerals(std::map<Mineral,int> min_used)
{
  if (!has_minerals(min_used)) {
    return false;
  }

  for (std::map<Mineral,int>::iterator it = min_used.begin();
       it != min_used.end();
       it++) {
    minerals[it->first] -= it->second;
  }
  return true;
}

void City::gain_resource(Resource res, int amount)
{
  resources[res] += amount;
}

void City::gain_resource(Resource_amount res)
{
  gain_resource(res.type, res.amount);
}

void City::gain_resources(std::vector<Resource_amount> res_used)
{
  for (int i = 0; i < res_used.size(); i++) {
    gain_resource( res_used[i] );
  }
}

void City::gain_resources(std::map<Resource,int> res_used)
{
  for (std::map<Resource,int>::iterator it = res_used.begin();
       it != res_used.end();
       it++) {
    gain_resource( it->first, it->second );
  }
}

void City::gain_mineral(Mineral min, int amount)
{
  minerals[min] += amount;
}

void City::gain_mineral(Mineral_amount min)
{
  gain_mineral(min.type, min.amount);
}

void City::gain_minerals(std::vector<Mineral_amount> min_used)
{
  for (int i = 0; i < min_used.size(); i++) {
    gain_mineral( min_used[i] );
  }
}

void City::gain_minerals(std::map<Mineral,int> min_used)
{
  for (std::map<Mineral,int>::iterator it = min_used.begin();
       it != min_used.end();
       it++) {
    gain_mineral( it->first, it->second );
  }
}

