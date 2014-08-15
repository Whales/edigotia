#include <map>
#include "area.h"

Area::Area(Area_type T, Point P)
{
  type = T;
  pos = P;
  open = true;
}

std::map<Resource,int> Area::get_maintenance()
{
  std::map<Resource,int> ret;
  Area_datum* data = Area_data[type];

  if (!data) {  // Should never ever happen, but why not
    return ret;
  }

  Building_datum* bd_data = Building_data[data->building];

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
