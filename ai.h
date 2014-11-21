#ifndef _AI_H_
#define _AI_H_

#include "resource.h"
#include <vector>


// City_role is an attempt to abstract what makes an AI city
// "roleial."  It primarily affects their imports and exports, but may also
// affect actions as well.

enum City_role
{
  CITY_ROLE_NULL = 0,     // No role

  CITY_ROLE_FARMING,      // Food production - exports lots of food
  CITY_ROLE_MINING,       // Mining - exports ores
  CITY_ROLE_LOGGING,      // Logging - exports wood

  CITY_ROLE_MAX
};

struct City_role_datum
{
  std::string name;

  std::vector<Resource> resource_exports;
  std::vector<Resource> resource_imports;
  std::vector<Mineral>  mineral_exports;
  std::vector<Mineral>  mineral_imports;
};

// These are in ai_data.cpp
extern City_role_datum* City_role_data[CITY_ROLE_MAX];
void init_city_roles();

#endif
