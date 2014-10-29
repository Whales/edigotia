#include "ai_city.h"

AI_City::AI_City()
{
  type = CITY_TYPE_CITY;

  for (int i = 0; i < CIT_MAX; i++) {
    population[i].type = Citizen_type(i);
  }
  population[CIT_PEASANT].add_citizens(150);
}

AI_City::~AI_City()
{
}

void AI_City::set_name()
{
  if (race == RACE_NULL) {
    name = "Abandoned City";
  } else {
    Race_datum* race_dat = Race_data[race];
    name = race_dat->get_city_name();
  }
}

// TODO: This is a duplicate of City::get_total_population().  They should
//       probably both be children of the same parent class.
// type defaults to CIT_NULL
int AI_City::get_total_population(Citizen_type type)
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

// TODO: This is a duplicate of City::get_glyph().  They should probably both be
//       children of the same parent class.
glyph AI_City::get_glyph()
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
