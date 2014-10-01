#include "building.h"

Building_datum* Building::get_building_datum()
{
  return Building_data[type];
}
