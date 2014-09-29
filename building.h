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
  BUILD_KEEP,

// Basic resources
  BUILD_FARM,
  BUILD_QUARRY,
  BUILD_MINE,
  BUILD_CAMP,
  BUILD_SAWMILL,

// Military
  BUILD_BARRACKS,

  BUILD_MAX
};

struct Building_datum
{
  Building_datum();
  ~Building_datum();

  std::string get_short_description();

  int uid;
  std::string name;
  bool plural;  // If true, name is treated as a plural
  std::vector<Resource_amount> build_costs;
  int upkeep;

  std::vector<Citizen_amount> housing;
  int military_support;

  Citizen_amount jobs;
  int build_time;
  std::vector<Resource_amount> production; // Per job, w/ skill of 5 (max)
  std::vector<Resource_amount> maintenance_cost;

// Things this building can construct
  std::vector<Resource> buildable;
};

struct Building
{
  Building_type type;
};

extern Building_datum* Building_data[BUILD_MAX];
void init_building_data();

#endif
