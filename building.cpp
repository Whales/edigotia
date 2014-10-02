#include "building.h"
#include "resource.h"
#include "window.h" // For debugmsg
#include <sstream>

Building::Building()
{
  type = BUILD_NULL;
  workers = 0;
  field_output = 0;
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

Building_datum::Building_datum()
{
  uid = -1;
  plural = false;
  upkeep = 0;
  military_support = 0;
  build_time = 0;
}

Building_datum::~Building_datum()
{
}

std::string Building_datum::get_short_description()
{
  std::stringstream ret;

  ret << "<c=white>" << name << "<c=/>" << std::endl;

  ret << "<c=yellow>Build time: " << build_time << " days.<c=/>" << std::endl;
  if (!build_costs.empty()) {
    ret << "<c=yellow>Build cost: ";
    for (int i = 0; i < build_costs.size(); i++) {
      ret << resource_name( build_costs[i].type ) << " x " <<
             build_costs[i].amount;
      if (i + 1 < build_costs.size()) {
        ret << ", ";
      }
    }
    ret << "<c=/>" << std::endl;
  }

  if (!maintenance_cost.empty()) {
    ret << "<c=ltred>Maintenance cost: ";
    for (int i = 0; i < maintenance_cost.size(); i++) {
      ret << resource_name( maintenance_cost[i].type ) << " x " <<
             maintenance_cost[i].amount;
      if (i + 1 < maintenance_cost.size()) {
        ret << ", ";
      }
    }
    ret << "<c=/>" << std::endl;
  }

  if (!housing.empty()) {
    ret << "<c=brown>Houses ";
    for (int i = 0; i < housing.size(); i++) {
      std::string cit_name = citizen_type_name(housing[i].type, true);
      ret << housing[i].amount << " " << cit_name << "<c=/>" << std::endl;
    }
  }

  if (jobs.amount > 0) {
    ret << "<c=cyan>Employs " << jobs.amount << " " <<
           citizen_type_name(jobs.type) << "s.<c=/>" << std::endl;
    if (!production.empty()) {
      ret << "<c=ltgreen>Each worker produces: ";
      for (int i = 0; i < production.size(); i++) {
        ret << resource_name( production[i].type ) << " x " <<
               production[i].amount;
        if (i + 1 < production.size()) {
          ret << ", ";
        }
      }
      ret << "<c=/>" << std::endl;
    }
  }

  if (!buildable.empty()) {
    ret << "<c=ltblue>Constructs: ";
    for (int i = 0; i < buildable.size(); i++) {
      ret << resource_name( buildable[i] );
      if (i + 1 < buildable.size()) {
        ret << ", ";
      }
    }
    ret << "<c=/>" << std::endl;
  }

  return ret.str();
}

bool Building_datum::add_production(Resource type, int amount)
{
  if (amount == INFINITE_RESOURCE) {
    debugmsg("%s produces infinite %s - this is invalid.",
             name.c_str(), resource_name(type).c_str());
    return false;
  } else if (amount == 0) {
    debugmsg("%s produces zero %s - this is invalid.",
             name.c_str(), resource_name(type).c_str());
    return false;
  } else if (amount < 0) {
    debugmsg("%s produces negative %s - this is invalid.",
             name.c_str(), resource_name(type).c_str());
    return false;
  }

  production.push_back( Resource_amount(type, amount) );
  return true;
}
