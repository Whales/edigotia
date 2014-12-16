#ifndef _AI_H_
#define _AI_H_

#include "resource.h"
#include "race.h"
#include <vector>

// City_role is an attempt to abstract what makes an AI city
// "roleial."  It primarily affects their imports and exports, but may also
// affect actions as well.

enum City_role
{
  CITY_ROLE_NULL = 0,     // No role

  CITY_ROLE_FARMING,      // Food production - exports lots of food
  CITY_ROLE_HUNTING,      // Exports food, leather and fur
  CITY_ROLE_LIVESTOCK,    // Exports food, leather, fur, and animals
  CITY_ROLE_MINING,       // Mining - exports ores
  CITY_ROLE_LOGGING,      // Logging - exports wood

  CITY_ROLE_MAX
};

struct City_role_datum
{
  City_role_datum();

  std::string name;

// skill defaults to SKILL_NULL; if it's something else, then we won't use this
// role unless our race's skill is at least skill_req
  Race_skill skill;
  int skill_req;

/* food_percentage is the amount of our required food that we produce.
 * If it's less than 100, we need to import some food.  If it's more than 100,
 * we'reaiming to exportsome food.  If it's exactly 100, we will try to produce
 * exactly what we need, but may wind up some some import or exports anyway.
 */
  int food_percentage;

  std::vector<Resource> resource_exports;
  std::vector<Resource> resource_imports;
  std::vector<Mineral>  mineral_exports;
  std::vector<Mineral>  mineral_imports;
};

// These are in ai_data.cpp
extern City_role_datum* City_role_data[CITY_ROLE_MAX];
void init_city_roles();

#endif
