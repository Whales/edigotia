#include "city.h"

Citizens::Citizens()
{
  count         = 0;
  employed      = 0;
  wealth        = 0;
  tax_morale    = 0;
  morale_points = 0;
  starvation    = 0;
}

Citizens::~Citizens()
{
}

int Citizens::get_unemployed()
{
  if (employed >= count) {
    return 0;
  }
  return count - employed;
}

int Citizens::get_income()
{
// Obviously, don't include income from government jobs (i.e. areas/buildings)
  int ret = 0;

// Idle citizens do produce income though!
  int idle = get_unemployed();
  ret += (idle * citizen_idle_income(type)) / 100;

// TODO: Other sources of income?

  return ret;
}

int Citizens::get_morale_percentage()
{
  if (count == 0) {
    return 0;
  }
  return tax_morale + (100 * morale_points) / count;
}

int Citizens::get_starvation_chance()
{
  if (count == 0) {
    return 0;
  }
  return starvation / count;
}

void Citizens::add_citizens(int amount)
{
  if (amount <= 0) {
    return;
  }
  count += amount;
  morale_points += 50 * amount; // Each one comes into the world with 50% morale
}

void Citizens::remove_citizens(int amount)
{
  if (amount <= 0) {
    return;
  }
// get_morale_percentage() basically returns the morale points per citizen.
// When we remove citizens, we need to remove their morale points too.
  morale_points -= amount * get_morale_percentage();
// Ditto starvation.
  starvation    -= amount * get_starvation_chance();
  count -= amount;
  if (count < 0) {
    morale_points = 0;
    count = 0;
  }
}

City::City()
{
  type = CITY_TYPE_CITY;
  race = RACE_NULL;
  game = NULL;
  for (int i = 0; i < CIT_MAX; i++) {
    population[i].type = Citizen_type(i);
  }
}

City::~City()
{
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

void City::set_game(Game* G)
{
  if (!G) {
    debugmsg("City::set_game(NULL) called!");
    return;
  }

  game = G;
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

