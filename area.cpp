#include "area.h"
#include "window.h"       // For debugmsg()
#include "city.h"         // For Area::close()
#include "player_city.h"  // For Area::close() and Area::reopen()
#include "stringfunc.h"   // For no_caps() and trim()
#include <map>

Area_datum::Area_datum()
{
  name = "Uninitialized Area";
  uid = -1;
  category = AREACAT_NULL;
  building = BUILD_NULL;
  unlockable = false;
}

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

void Area::close(City* city)
{
  if (!city) {
    debugmsg("%s called Area::close(NULL)!", get_name().c_str());
    return;
  }

  if (!open) {  // We're already closed!
    return;
  }

  open = false;
  Citizen_type cit_type = building.get_job_citizen_type();

// Handle any stuff specific to farms, mines, etc
  for (int i = 0; i < building.crops_grown.size(); i++) {
    if (building.crops_grown[i].amount != HIDDEN_RESOURCE) {
      building.crops_grown[i].amount = 0;
    }
  }

  for (int i = 0; i < building.minerals_mined.size(); i++) {
    if (building.minerals_mined[i].amount != HIDDEN_RESOURCE) {
      building.minerals_mined[i].amount = 0;
    }
  }

  if (city->is_player_city()) {
    Player_city* p_city = static_cast<Player_city*>(city);
    p_city->fire_citizens(cit_type, building.workers, &building);
  }
}

void Area::auto_hire(Player_city* city)
{
// First, attempt to hire citizens.
  int num_jobs = building.get_total_jobs();
  Citizen_type cit_type = building.get_job_citizen_type();
  if (city->employ_citizens(cit_type, num_jobs, &building)) {
    city->add_message(MESSAGE_MINOR, "%d %s have started work at the %s.",
                      num_jobs, citizen_type_name(cit_type, true).c_str(),
                      get_name().c_str());

// If it's a farm, we need to set crops to grow.
    if (building.produces_resource(RES_FARMING)) {
// Find whatever crop produces the most food.
      int best_index = -1, best_food = -1;
      for (int i = 0; i < building.crops_grown.size(); i++) {
        Crop crop = building.crops_grown[i].type;
        int food = Crop_data[crop]->food;
        if (food > best_food) {
          best_index = i;
          best_food = food;
        }
      }
// We found a good crop to grow!
      if (best_index >= 0) {
        Crop crop = building.crops_grown[best_index].type;
        Crop_datum* crop_dat = Crop_data[crop];
        city->add_message(MESSAGE_MINOR, "Our %s is now growing %s.",
                          get_name().c_str(), crop_dat->name.c_str());
        building.crops_grown[best_index].amount += num_jobs;
      } else {
        city->add_message(MESSAGE_MAJOR,
                          "Our %s needs to select a crop to grow.",
                          get_name().c_str());
      }
    }
// Mines need to have minerals chosen
    if (building.produces_resource(RES_MINING)) {
// Find whatever mineral is worth the most
      int best_index = -1, best_value = -1;
      for (int i = 0; i < building.minerals_mined.size(); i++) {
        Mineral mineral = building.minerals_mined[i].type;
        int value = Mineral_data[mineral]->value;
        if (building.minerals_mined[i].amount != HIDDEN_RESOURCE && 
            value > best_value) {
          best_index = i;
          best_value = value;
        }
      }
// We found a good mineral to mine!
      if (best_index >= 0) {
        Mineral min = building.minerals_mined[best_index].type;
        Mineral_datum* min_dat = Mineral_data[min];
        city->add_message(MESSAGE_MINOR, "Our %s is now mining %s.",
                          get_name().c_str(), min_dat->name.c_str());
        building.minerals_mined[best_index].amount += num_jobs;
      } else {
        city->add_message(MESSAGE_MAJOR,
                          "Our %s needs to select a mineral to mine.",
                          get_name().c_str());
      }
    }

  } else if (num_jobs > 0) {
// We failed to hire citizens.
    city->add_message(MESSAGE_MINOR, "Our %s could not hire citizens.",
                      get_name().c_str());
  }

}

bool Area::under_construction()
{
  return (building.construction_left > 0);
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

int Area::get_destroy_cost()
{
  Building_datum* build_dat = get_building_datum();
  if (build_dat) {
    return build_dat->destroy_cost;
  }
  return 0;
}

int Area::get_reopen_cost()
{
  Building_datum* build_dat = get_building_datum();
  if (build_dat) {
    int cost = 0;
    for (int i = 0; cost == 0 && i < build_dat->build_costs.size(); i++) {
      if (build_dat->build_costs[i].type == RES_GOLD) {
        cost = build_dat->build_costs[i].amount / 10;
      }
    }
    return cost;
  }
  return 0;
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

Area_category lookup_area_category(std::string name)
{
  name = no_caps( trim( name ) );
  for (int i = 0; i < AREACAT_MAX; i++) {
    Area_category ret = Area_category(i);
    if (name == area_category_name(ret)) {
      return ret;
    }
  }
  return AREACAT_NULL;
}

std::string area_category_name(Area_category category)
{
  switch (category) {
    case AREACAT_NULL:      return "NULL";
    case AREACAT_HOUSING:   return "housing";
    case AREACAT_FOOD:      return "food";
    case AREACAT_RESOURCES: return "resources";
    case AREACAT_MILITARY:  return "military";
    case AREACAT_MAX:       return "BUG - AREACAT_MAX";
    default:                return "BUG - Unnamed Area_category";
  }
  return "BUG - Escaped area_category_name() switch!";
}

Building_datum* get_building_for(Area_type area)
{
  Area_datum* adat = Area_data[area];
  if (!adat || adat->building == BUILD_NULL) {
    return NULL;
  }
  return Building_data[ adat->building ];
}
