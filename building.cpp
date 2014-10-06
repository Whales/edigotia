#include "building.h"
#include "resource.h"
#include "window.h" // For debugmsg
#include <sstream>

Building::Building()
{
  pos = Point(-1, -1);
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

int Building::get_empty_fields()
{
  if (!produces_resource(RES_FARMING)) {
    return 0;
  }
  int max_fields = get_total_jobs();
  int fields_used = 0;
  for (int i = 0; i < crops_grown.size(); i++) {
    fields_used += crops_grown[i].amount;
  }
  return (max_fields - fields_used);
}

int Building::get_empty_shafts()
{
  if (!produces_resource(RES_MINING)) {
    return 0;
  }
  int max_shafts = get_total_jobs();
  int shafts_used = 0;
  for (int i = 0; i < minerals_mined.size(); i++) {
    if (minerals_mined[i].amount != HIDDEN_RESOURCE) {
      shafts_used += minerals_mined[i].amount;
    }
  }
  return (max_shafts - shafts_used);
}

Building_datum* Building::get_building_datum()
{
  return Building_data[type];
}

std::string Building::get_name()
{
  return get_building_datum()->name;
}

bool Building::produces_resource(Resource res)
{
  Building_datum* build_dat = get_building_datum();
  return build_dat->produces_resource(res);
}

int Building::amount_produced(Resource res)
{
  Building_datum* build_dat = get_building_datum();
  return build_dat->amount_produced(res);
}

// cit_type defaults to CIT_NULL
int Building::get_total_jobs(Citizen_type cit_type)
{
  return get_building_datum()->get_total_jobs(cit_type);
}

// cit_type defaults to CIT_NULL
int Building::get_available_jobs(Citizen_type cit_type)
{
  int total = get_total_jobs(cit_type);
  if (workers >= total) {
    return 0;
  }
  return total - workers;
}

// cit_type defaults to CIT_NULL
int Building::get_filled_jobs(Citizen_type cit_type)
{
// Ensure that we actually hire citizens of that type
  if (cit_type != CIT_NULL && get_total_jobs(cit_type) == 0) {
    return 0;
  }
  return workers;
}

Citizen_type Building::get_job_citizen_type()
{
  return get_building_datum()->jobs.type;
}

int Building::get_upkeep()
{
  return get_building_datum()->upkeep;
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

bool Building_datum::produces_resource(Resource res)
{
  for (int i = 0; i < production.size(); i++) {
    if (production[i].type == res) {
      return true;
    }
  }
  return false;
}

int Building_datum::amount_produced(Resource res)
{
  int ret = 0;
  for (int i = 0; i < production.size(); i++) {
    if (production[i].type == res) {
      ret += production[i].amount;
    }
  }
  return ret;
}

// cit_type defaults to CIT_NULL
int Building_datum::get_total_jobs(Citizen_type cit_type)
{
  if (cit_type == CIT_NULL || cit_type == jobs.type) {
    return jobs.amount;
  }
  return 0;
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
