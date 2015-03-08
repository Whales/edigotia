#include "area.h"
#include "window.h"       // For debugmsg()
#include "city.h"         // For Area::close()
#include "player_city.h"  // For Area::close() and Area::reopen()
#include "stringfunc.h"   // For no_caps() and trim()
#include <map>
#include <sstream>

Area_datum::Area_datum()
{
  name = "Uninitialized Area";
  uid = -1;
  category = AREACAT_NULL;
  building = BUILD_NULL;
  buildings_supported = 0;
  unlockable = false;
}

Building_datum* Area_datum::get_building_datum()
{
  return Building_data[building];
}

std::string Area_datum::generate_help_text()
{
  std::stringstream ret;

  ret << "<c=white>" << capitalize(name) << "<c=/> " <<
         symbol.text_formatted() << std::endl;
  ret << "<c=magenta>Type: " << capitalize( area_category_name(category) ) <<
         "<c=/>" << std::endl;

  Building_datum* bldg = get_building_datum();
  if (!bldg) {
    ret << "<c=red>ERROR<c=/>: No building datum for area " << name << "!";
  } else {
// 1st true: turn on help links.  2nd true: format for area
    ret << bldg->get_short_description(true, true);
  }

  if (buildings_supported > 0) {
    ret << "<c=ltred>Supports " << buildings_supported << " <link=building>" <<
           "buildings</link>" << std::endl;
  }
  if (unlockable) {
    ret << "<link=unlockables>Unlock Condition</link>: <c=white>" <<
           unlock_condition.get_description() << "<c=/>" << std::endl;
  }

  if (bldg) {
    ret << std::endl << bldg->description;
  }

  return ret.str();
}

Area::Area(Area_type T, Point P)
{
  type = T;
  building.set_type(get_building_type());
  pos = P;
  building.pos = P;
  building.open = true;
  building.construction_left = 0;
}

std::string Area::save_data()
{
  std::stringstream ret;

  ret << type << " ";
  ret << pos.x << " " << pos.y << " ";
  ret << std::endl;
  ret << building.save_data();
  ret << std::endl;

  return ret.str();
}

bool Area::load_data(std::istream& data)
{
  int tmptype;
  data >> tmptype;
  if (tmptype <= 0 || tmptype >= AREA_MAX) {
    debugmsg("Area loaded type 5d (range is 1 to %d).", tmptype, AREA_MAX - 1);
    return false;
  }
  type = Area_type(tmptype);

  data >> pos.x >> pos.y;
  if (!building.load_data(data)) {
    debugmsg("Area (%s) failed to load building data.", get_name().c_str());
    return false;
  }

  return true;
}

void Area::make_queued()
{
  Building_datum* bd_data = get_building_datum();
  if (!bd_data) { // Should never happen
    return;
  }

  building.open = false;
  building.construction_left = bd_data->build_time;
}

void Area::close(City* city)
{
  if (!city) {
    debugmsg("%s called Area::close(NULL)!", get_name().c_str());
    return;
  }

  if (!is_open()) {  // We're already closed!
    return;
  }

  building.close(city);
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

bool Area::is_open()
{
  return building.open;
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
  return building.get_destroy_cost();
}

int Area::get_reopen_cost()
{
  return building.get_reopen_cost();
}

std::map<Resource,int> Area::get_maintenance()
{
  return building.get_maintenance();
}

bool Area::produces_resource(Resource res)
{
  return (is_open() && get_building_datum()->produces_resource(res));
}

int Area::amount_produced(Resource res)
{
  if (!is_open()) {
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
    case AREACAT_NULL:            return "NULL";
    case AREACAT_INFRASTRUCTURE:  return "infrastructure";
    case AREACAT_HOUSING:         return "housing";
    case AREACAT_FOOD:            return "food";
    case AREACAT_RESOURCES:       return "resources";
    case AREACAT_MILITARY:        return "military";
    case AREACAT_MAX:             return "BUG - AREACAT_MAX";
    default:                      return "BUG - Unnamed Area_category";
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
