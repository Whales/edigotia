#include "building.h"
#include "area.h"
#include "map.h"
#include "resource.h"
#include "race.h"
#include "animal.h"
#include "ai.h"
#include "help.h"
#include "globals.h"

bool init_all_data()
{
  init_building_data();
  init_area_data();
  init_terrain_data();
  init_map_type_data();
  init_resource_data();
  init_animal_data();
  init_races();
  init_city_roles();

  HELP = new Help_database;
  init_help();

  return true;
}
