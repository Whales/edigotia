#include "building.h"
#include "resource.h"
#include "city.h" // Needed in Building::amount_built()
#include "window.h" // For debugmsg
#include "stringfunc.h" // Needed in lookup_building_category()
#include <sstream>

// R defaults to RES_NULL, A defaults to 1
Recipe::Recipe(Resource R, int A)
{
  result = Resource_amount(R, A);
}

std::string Recipe::get_name()
{
  if (!name.empty()) {
    return name;
  }

  Resource res = get_resource();
  return resource_name( res );
}

Resource Recipe::get_resource()
{
  return result.type;
}

Resource Recipe_amount::get_resource()
{
  return recipe.get_resource();
}

Building::Building()
{
  pos = Point(-1, -1);
  construction_left = 0;
  type = BUILD_NULL;
  workers = 0;
  field_output = 0;
  shaft_output = 0;
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

void Building::make_queued()
{
  Building_datum* datum = get_building_datum();
  if (!datum) {
    debugmsg("Building::make_queued() called get_building_datum() and got \
NULL!");
    return;
  }

  construction_left = datum->build_time;
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

// res defaults to RES_NULL
bool Building::produces_resource(Resource res)
{
  Building_datum* build_dat = get_building_datum();
  return build_dat->produces_resource(res);
}

// res defaults to RES_NULL
bool Building::builds_resource(Resource res)
{
  Building_datum* build_dat = get_building_datum();
  return build_dat->builds_resource(res);
}

  
int Building::amount_produced(Resource res)
{
  Building_datum* build_dat = get_building_datum();
  return build_dat->amount_produced(res);
}

int Building::amount_built(Resource res, City* city)
{
  if (!city) {
    return 0;
  }

  if (build_queue.empty()) {
    return 0;
  }
// We only need to check the first item in our build_queue.
  Recipe rec = build_queue[0].recipe;
  if (rec.get_resource() == res &&
      city->has_resources(rec.resource_ingredients) &&
      city->has_minerals (rec.mineral_ingredients )) {
    int ret = rec.result.amount;
    ret *= workers;
// Multiple by units_per_day or divide by days_per_unit.
    if (rec.units_per_day != 0) {
      ret *= rec.units_per_day;
    } else if (rec.days_per_unit != 0) {
      ret /= rec.days_per_unit;
    }
    return ret;
  }

  return 0;
}

int Building::livestock_space()
{
  return get_building_datum()->livestock_space;
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

std::map<Resource,int> Building::get_maintenance()
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

  if (ret.count(RES_GOLD)) {
    ret[RES_GOLD] += bd_data->upkeep;
  } else {
    ret[RES_GOLD] = bd_data->upkeep;
  }

  return ret;
}

// cit_type defaults to CIT_NULL
int Building::get_housing(Citizen_type cit_type)
{
  return get_building_datum()->get_housing(cit_type);
}

Building_datum::Building_datum()
{
  uid = -1;
  plural = false;
  category = BUILDCAT_NULL; // i.e. is an Area-only building
  upkeep = 0;
  military_support = 0;
  livestock_space = 0;
  build_time = 0;
  destroy_cost = 0;
  unlockable = false;
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

  if (!recipes.empty()) {
    ret << "<c=ltblue>Constructs: ";
    for (int i = 0; i < recipes.size(); i++) {
      ret << resource_name( recipes[i].get_resource() );
      if (i + 1 < recipes.size()) {
        ret << ", ";
      }
    }
    ret << "<c=/>" << std::endl;
  }

  return ret.str();
}

// res defaults to RES_NULL
bool Building_datum::produces_resource(Resource res)
{
  if (res == RES_NULL) {
    return !(production.empty());
  }

  for (int i = 0; i < production.size(); i++) {
    if (production[i].type == res) {
      return true;
    }
  }
  return false;
}

// res defaults to RES_NULL
bool Building_datum::builds_resource(Resource res)
{
  if (res == RES_NULL) {
    return !(recipes.empty());
  }

  for (int i = 0; i < recipes.size(); i++) {
    if (recipes[i].result.type == res) {
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

// cit_type defaults to CIT_NULL
int Building_datum::get_housing(Citizen_type cit_type)
{
  int ret = 0;
  for (int i = 0; i < housing.size(); i++) {
    if (cit_type == CIT_NULL || cit_type == housing[i].type) {
      ret += housing[i].amount;
    }
  }
  return ret;
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

Building_category lookup_building_category(std::string name)
{
  name = no_caps( trim( name ) );
  for (int i = 0; i < BUILDCAT_MAX; i++) {
    Building_category ret = Building_category(i);
    if (no_caps( trim( building_category_name(ret) ) ) == name) {
      return ret;
    }
  }
  return BUILDCAT_NULL;
}

std::string building_category_name(Building_category category)
{
  switch (category) {
    case BUILDCAT_NULL:           return "NULL";
    case BUILDCAT_MANUFACTURING:  return "manufacturing";
    case BUILDCAT_MAX:            return "BUG - BUILDCAT_MAX";
    default:  return "Unnamed Building_category";
  }
  return "BUG - Escaped building_category_name() switch";
}
