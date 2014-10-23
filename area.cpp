#include <map>
#include "area.h"

Building_datum* Area_datum::get_building_datum()
{
  return Building_data[building];
}

Area::Area(Area_type T, Point P)
{
  type = T;
  building.set_type(get_building_type());
  pos = P;
  building.pos = P;
  open = true;
  building.construction_left = 0;
}

void Area::make_queued()
{
  Building_datum* bd_data = get_building_datum();
  if (!bd_data) { // Should never happen
    return;
  }

  open = false;
  building.construction_left = bd_data->build_time;
}

Area_datum* Area::get_area_datum()
{
  return Area_data[type];
}

std::string Area::get_name()
{
  return Area_data[type]->name;
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
  return building.get_maintenance();
}

bool Area::produces_resource(Resource res)
{
  return (open && get_building_datum()->produces_resource(res));
}

int Area::amount_produced(Resource res)
{
  if (!open) {
    return 0;
  }
  return get_building_datum()->amount_produced(res);
}

Building_datum* get_building_for(Area_type area)
{
  Area_datum* adat = Area_data[area];
  if (!adat || adat->building == BUILD_NULL) {
    return NULL;
  }
  return Building_data[ adat->building ];
}
