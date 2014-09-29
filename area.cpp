#include <map>
#include "area.h"

Building_datum* Area_datum::get_building_datum()
{
  return Building_data[building];
}

Area::Area(Area_type T, Point P)
{
  type = T;
  pos = P;
  open = true;
  construction_left = 0;
}

void Area::make_queued()
{
  Building_datum* bd_data = get_building_datum();
  if (!bd_data) { // Should never happen
    return;
  }

  construction_left = bd_data->build_time;
}

Area_datum* Area::get_area_datum()
{
  return Area_data[type];
}

Building_type Area::get_building_type()
{
  Area_datum* datum = get_area_datum();
  if (!datum) {
    return BUILD_NULL;
  }
  return datum->building;
}

Building_datum* Area::get_building_datum()
{
  Area_datum* datum = get_area_datum();
  if (!datum) {
    return Building_data[BUILD_NULL]; // Safer than returning NULL
  }
  return datum->get_building_datum();
}

std::map<Resource,int> Area::get_maintenance()
{
  std::map<Resource,int> ret;

  Building_datum* bd_data = get_building_datum();

  if (!bd_data) {  // Should never ever happen, but why not
    return ret;
  }

  for (int i = 0; i < bd_data->maintenance_cost.size(); i++) {
    Resource_amount res = bd_data->maintenance_cost[i];
    if (ret.count(res.type)) {
      ret[res.type] += res.amount;
    } else {
      ret[res.type] = res.amount;
    }
  }

  return ret;
}

Building_datum* get_building_for(Area_type area)
{
  Area_datum* adat = Area_data[area];
  if (!adat || adat->building == BUILD_NULL) {
    return NULL;
  }
  return Building_data[ adat->building ];
}
