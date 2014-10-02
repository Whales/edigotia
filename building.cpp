#include "building.h"

Building::Building()
{
  type = BUILD_NULL;
  workers = 0;
}

Building::~Building()
{
}

void Building::set_type(Building_type new_type)
{
  type = new_type;
  build_queue.clear();
  crops_grown.clear();
  minerals_mined.clear();
}

Building_datum* Building::get_building_datum()
{
  return Building_data[type];
}
