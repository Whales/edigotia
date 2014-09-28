#include "building.h"
#include "area.h"
#include "map.h"
#include "resource.h"

bool init_all_data()
{
  init_building_data();
  init_area_data();
  init_terrain_data();
  init_map_type_data();
  init_crop_and_mineral_data();

  return true;
}
