#ifndef _BUILDING_H_
#define _BUILDING_H_

#include <string>
#include <vector>
#include "citizen.h"
#include "resource.h"

enum Building_type
{
  BUILD_NULL = 0,

// Housing
  BUILD_HOVEL,
  BUILD_HOUSE,
  BUILD_MANOR,
  BUILD_PALACE,

// Basic resources
  BUILD_FARM,
  BUILD_MINE,
  BUILD_CAMP,

  BUILD_MAX
};

struct Building_datum
{
  int uid;
  std::string name;
  std::vector<Resource_cost> build_costs;
  int upkeep;

  Citizen_amount housing;

  int jobs;
  std::vector<Resource_cost> flat_production; // Per job, w/ skill of 5 (max)

  std::vector<Resource> buildable;
};

struct Building
{
  Building_type type;
};

extern Building_datum* Building_data[BUILD_MAX];
void init_building_data();

#endif
