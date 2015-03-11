#include "building.h"
#include "resource.h"
#include "city.h" // Needed in Building::amount_built()
#include "window.h" // For debugmsg
#include "stringfunc.h" // Needed in lookup_building_category()
#include "player_city.h"  // For close()
#include "rng.h"
#include <sstream>

// R defaults to RES_NULL, A defaults to 1
Recipe::Recipe(Resource R, int A)
{
  result = Resource_amount(R, A);
  units_per_day = 1;
  days_per_unit = 0;
  max_deficit = -10;
}

std::string Recipe::save_data()
{
  std::stringstream ret;

// Since some names are multi-word, use ! as a terminator
  ret << name << " ! ";
  ret << result.type << " " << result.amount << " ";
  ret << units_per_day << " ";
  ret << days_per_unit << " ";
  ret << std::endl;

  ret << resource_ingredients.size() << " ";
  for (int i = 0; i < resource_ingredients.size(); i++) {
    ret << resource_ingredients[i].type << " " <<
           resource_ingredients[i].amount << " ";
  }
  ret << std::endl;

  ret << mineral_ingredients.size() << " ";
  for (int i = 0; i < mineral_ingredients.size(); i++) {
    ret << mineral_ingredients[i].type << " " <<
           mineral_ingredients[i].amount << " ";
  }
  ret << std::endl;

  return ret.str();
}

bool Recipe::load_data(std::istream& data)
{
  name = "";

  std::string tmpstr;
  while (tmpstr != "!") {
    data >> tmpstr;
    if (tmpstr != "!") {
      if (!name.empty()) {
        name = name + " ";
      }
      name = name + tmpstr;
    }
  }

  int tmptype, tmpnum;
  data >> tmptype >> tmpnum;
  if (tmptype <= 0 || tmptype >= RES_MAX) {
    debugmsg("Recipe loaded result of type %d (range is 1 to %d).",
             tmptype, RES_MAX - 1);
    return false;
  }
  result = Resource_amount( Resource(tmptype), tmpnum );

  data >> units_per_day >> days_per_unit;

  resource_ingredients.clear();
  int num_res;
  data >> num_res;
  for (int i = 0; i < num_res; i++) {
    data >> tmptype >> tmpnum;
    if (tmptype <= 0 || tmptype >= RES_MAX) {
      debugmsg("Recipe loaded res ingredient of type %d (range is 1 to %d).",
               tmptype, RES_MAX - 1);
      return false;
    }
    resource_ingredients.push_back(Resource_amount(Resource(tmptype), tmpnum));
  }

  mineral_ingredients.clear();
  int num_min;
  data >> num_min;
  for (int i = 0; i < num_min; i++) {
    data >> tmptype >> tmpnum;
    if (tmptype <= 0 || tmptype >= MINERAL_MAX) {
      debugmsg("Recipe loaded min ingredient of type %d (range is 1 to %d).",
               tmptype, MINERAL_MAX - 1);
      return false;
    }
    mineral_ingredients.push_back( Mineral_amount( Mineral(tmptype), tmpnum ) );
  }

  return true;
}

std::string Recipe::get_name()
{
  if (!name.empty()) {
    return name;
  }

  Resource res = get_resource();
  return Resource_data[ res ]->name;
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
  hunter_level = 0;
  hunting_target = ANIMAL_NULL;
  hunting_action = ANIMAL_ACT_KILL;
}

Building::~Building()
{
}

std::string Building::save_data()
{
  std::stringstream ret;

  ret << int(type) << " ";
  ret << open << " ";
  ret << pos.x << " " << pos.y << " ";
  ret << construction_left << " ";
  ret << workers << " ";

  ret << field_output << " ";
  ret << crops_grown.size() << " ";
  for (int i = 0; i < crops_grown.size(); i++) {
    ret << crops_grown[i].type << " " << crops_grown[i].amount << " ";
  }
  ret << std::endl;

  ret << shaft_output << " ";
  ret << minerals_mined.size() << " ";
  for (int i = 0; i < minerals_mined.size(); i++) {
    ret << minerals_mined[i].type << " " << minerals_mined[i].amount << " ";
  }
  ret << std::endl;

  ret << hunter_level << " ";
  ret << hunting_target << " ";
  ret << hunting_action << std::endl;

  ret << build_queue.size() << " ";
  for (int i = 0; i < build_queue.size(); i++) {
    ret << build_queue[i].recipe.save_data() << " " << build_queue[i].amount <<
           " ";
  }
  ret << std::endl;

  return ret.str();
}

bool Building::load_data(std::istream& data)
{
  int tmptype;
  data >> tmptype;
  if (tmptype <= 0 || tmptype >= BUILD_MAX) {
    debugmsg("Building loaded type of %d (range is 1 to %d).",
             tmptype, BUILD_MAX - 1);
    return false;
  }
  type = Building_type(tmptype);

  data >> open >> pos.x >> pos.y >> construction_left >> workers;

  crops_grown.clear();
  data >> field_output;
  int num_crops;
  data >> num_crops;
  for (int i = 0; i < num_crops; i++) {
    int tmpcrop, crop_amt;
    data >> tmpcrop >> crop_amt;
    if (tmpcrop <= 0 || tmpcrop >= CROP_MAX) {
      debugmsg("Building loaded crop %d/%d; type %d (range is 1 to %d).",
               i, num_crops, tmpcrop, CROP_MAX - 1);
      return false;
    }
    crops_grown.push_back( Crop_amount( Crop(tmpcrop), crop_amt ) );
  }

  minerals_mined.clear();
  data >> shaft_output;
  int num_mins;
  data >> num_mins;
  for (int i = 0; i < num_mins; i++) {
    int tmpmin, min_amt;
    data >> tmpmin >> min_amt;
    if (tmpmin <= 0 || tmpmin >= MINERAL_MAX) {
      debugmsg("Building loaded mineral %d/%d; type %d (range is 1 to %d).",
               i, num_mins, tmpmin, MINERAL_MAX - 1);
      return false;
    }
    minerals_mined.push_back( Mineral_amount( Mineral(tmpmin), min_amt ) );
  }

  data >> hunter_level;
  int tmptarget, tmpaction;
  data >> tmptarget >> tmpaction;
  if (tmptarget >= ANIMAL_MAX) {
    debugmsg("Building loaded hunting target %d (range is 1 to %d).",
             tmptarget, ANIMAL_MAX - 1);
    return false;
  }
  hunting_target = Animal(tmptarget);
  if (tmpaction >= ANIMAL_ACT_MAX) {
    debugmsg("Building loaded hunting action %d (range is 1 to %d).",
             tmpaction, ANIMAL_ACT_MAX - 1);
    return false;
  }
  hunting_action = Animal_action(tmpaction);

  int queue_size;
  data >> queue_size;
  for (int i = 0; i < queue_size; i++) {
    Recipe tmp_recipe;
    if (!tmp_recipe.load_data(data)) {
      debugmsg("Building failed to load recipe %d/%d.", i, queue_size);
      return false;
    }
    int recipe_amt;
    data >> recipe_amt;
    build_queue.push_back( Recipe_amount( tmp_recipe, recipe_amt ) );
  }

  return true;
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

void Building::close(City* city)
{
  if (!city) {
    debugmsg("%s called Building::close(NULL)!", get_name().c_str());
    return;
  }

  if (!open) {  // We're already closed!
    return;
  }

  open = false;
  Citizen_type cit_type = get_job_citizen_type();

// Handle any stuff specific to farms, mines, etc
  for (int i = 0; i < crops_grown.size(); i++) {
    if (crops_grown[i].amount != HIDDEN_RESOURCE) {
      crops_grown[i].amount = 0;
    }
  }

  for (int i = 0; i < minerals_mined.size(); i++) {
    if (minerals_mined[i].amount != HIDDEN_RESOURCE) {
      minerals_mined[i].amount = 0;
    }
  }

  if (city->is_player_city()) {
    Player_city* p_city = static_cast<Player_city*>(city);
    p_city->fire_citizens(cit_type, workers, this);
  }
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

void Building::discover_minerals(City* city)
{
  if (!city) {
    debugmsg("Building::discover_minerals(NULL) called!");
    return;
  }

  Map_tile* tile = city->map.get_tile(pos);
  if (!tile) {
    debugmsg("In Building::discover_minerals() our Map_tile is NULL!");
    return;
  }

  Player_city* pl_city = NULL;
  if (city->is_player_city()) {
    pl_city = static_cast<Player_city*>(city);
  }

  int skill = Race_data[city->get_race()]->skill_level[SKILL_MINING];
/* chance is our chance to TRY to find the mineral.  There's two rolls; a
 * 1-in-chance roll, and a roll against the amount of the mineral buried.  The
 * second roll is skipped if there's an infinite amount buried; and generally
 * speaking it's pretty easy anyway.
 */

  int chance = 102 - 20 * skill;

  for (int i = 0; i < minerals_mined.size(); i++) {
    Mineral_amount* min_amt = &(minerals_mined[i]);
    int amount_buried = tile->get_mineral_amount(min_amt->type);
    if (min_amt->amount == HIDDEN_RESOURCE && one_in(chance) &&
        (amount_buried == INFINITE_RESOURCE || rng(1, 20000) < amount_buried)) {
      min_amt->amount = 0;
      if (pl_city) {
        pl_city->add_message(MESSAGE_MAJOR, "Our mine has discovered %s!",
                             Mineral_data[min_amt->type]->name.c_str());
      }
    }
  }
}

int Building::get_max_hunt_prey()
{
  if (hunting_target == ANIMAL_NULL) {
    return 0;
  }
  Animal_datum* ani_dat = Animal_data[hunting_target];
  if (ani_dat->difficulty == 0) {
    debugmsg("Animal '%s' has difficulty 0!", ani_dat->name.c_str());
    return 0;
  }
  return (workers * hunter_level) / ani_dat->difficulty;
}
  
int Building::get_max_hunt_food()
{
  if (hunting_target == ANIMAL_NULL) {
    return 0;
  }
  Animal_datum* ani_dat = Animal_data[hunting_target];
  int num_caught = get_max_hunt_prey();
  int remainder = (workers * hunter_level) % ani_dat->difficulty;
  return num_caught * ani_dat->food_killed +
         (remainder * ani_dat->food_killed) / ani_dat->difficulty;
}

Building_datum* Building::get_building_datum()
{
  return Building_data[type];
}

std::string Building::get_name()
{
  return get_building_datum()->name;
}

// real defaults to false
std::map<Resource,int> Building::get_resource_production(City* city, bool real)
{
  if (!open) { // Closed buildings don't produce anything!
    return std::map<Resource,int>();
  }

  if (!city) {
    debugmsg("Building::get_resource_production(NULL) called!");
    return std::map<Resource,int>();
  }

  Player_city* pl_city = NULL;
  if (city->is_player_city()) {
    pl_city = static_cast<Player_city*>(city);
  }

  Building_datum* datum = get_building_datum();

  if (!datum) {
    debugmsg("Encountered NULL datum in Building::get_resource_production()!");
    return std::map<Resource,int>();
  }

// Start with a std::map - we'll turn it into a std::vector at the end.
  std::map<Resource,int> resource_gains;

  if (workers > 0) {
    Map_tile* tile = city->map.get_tile(pos);

// First, look at the production vector of our type.
    for (int i = 0; i < datum->production.size(); i++) {
      Resource_amount res_amt = datum->production[i];

      if (!Resource_data[res_amt.type]->meta) { // Gain non-meta resources
        resource_gains[res_amt.type] += res_amt.amount * workers;

      } else {  // Meta resources are more complicated!
        int skill = 0;
        switch (res_amt.type) {
          case RES_FARMING: {
            skill = Race_data[city->get_race()]->skill_level[SKILL_FARMING];
// Bonus resources are listed in our data as per 100 crops.  Thus, we store them
// in a seperate map, then divide by 100 after all crops are processed to avoid
// rounding as much as possible.
            std::map<Resource,int> bonus_resources;
            int total_food = 0;
            for (int n = 0; n < crops_grown.size(); n++) {
              int crop_count = crops_grown[n].amount;
              if (crop_count > 0) {
                Crop crop = crops_grown[n].type;
                Crop_datum* crop_dat = Crop_data[crop];
// Add food from the crop.
                int food = crop_dat->food;
                food *= crop_count;
                food *= field_output;
                total_food += food;
// Look for any non-food resources the crop produces.
                for (int m = 0; m < crop_dat->bonus_resources.size(); m++) {
                  Resource_amount res_amt = crop_dat->bonus_resources[m];
                  res_amt.amount *= crop_count;
                  res_amt.amount *= field_output;
// The amount needs to be divided by 100 since field_output is the terrain's
// farmability, which is a percentage (0 to 100, should be 0.0 to 1.0).
// (We'll divide by 100 again before adding bonus_resources to resource_gains,
//  since the Resource_amount is per 100 crops)
                  res_amt.amount /= 100;
                  bonus_resources[res_amt.type] += res_amt.amount;
                }
              } // if (amount > 0)
            } // for (int n = 0; n < crops_grown.size(); n++)
/* Crop_datum::food is per 100 units of the crop, so naturally we must divide by
 * 100.  Additionally, field_output is 100 times higher than it should be (since
 * terrain farmability is a percentage, 0 to 100) so we must divide by 100
 * again.  Combining both means dividing by 10,000.
 */
            total_food /= 10000;
            resource_gains[RES_FOOD] += total_food;
            for (std::map<Resource,int>::iterator it = bonus_resources.begin();
                 it != bonus_resources.end();
                 it++) {
// Here we only divide by 100, since terrain farmability is ignored.
              resource_gains[it->first] += it->second / 100;
            }
          } break;  // case RES_FARMING

          case RES_HUNTING:
// We need to let the city handle this, since it requires info on the Area.
            break;

          case RES_LOGGING: {
            skill = Race_data[city->get_race()]->skill_level[SKILL_FORESTRY];
            int wood_produced = workers * amount_produced(RES_LOGGING) * skill;
// Divide by 5 since skill is 1 to 5, with 5 meaning "no reduction"
            wood_produced /= 5;
            if (tile->wood != INFINITE_RESOURCE && tile->wood < wood_produced) {
// We've exhausted the tile's supply of wood!
              resource_gains[RES_WOOD] += tile->wood;
              if (real) {
                tile->clear_wood();
                close(city);
                if (pl_city) {
                  pl_city->add_message(MESSAGE_MAJOR,
                    "Our %s has cleared the %s and is now closed.",
                    get_name().c_str(),
                    tile->get_terrain_name().c_str());
                }
              }

            } else {  // We have NOT exhausted the supply of wood.
              resource_gains[RES_WOOD] += wood_produced;
              if (real && tile->wood != INFINITE_RESOURCE) {
                tile->wood -= wood_produced;
              }
            }
          } break; // case RES_LOGGING
        } // switch (res_amt.type)
      } // if (Resource_data[res_amt.type]->meta)
    } // for (int i = 0; i < datum->production.size(); i++)

// Next, if we have a build_queue attempt to build the first item in it
    if (!build_queue.empty()) {
      Recipe_amount* recipe_amt = &(build_queue[0]);
      Recipe* recipe = &(recipe_amt->recipe);
// Check if we can build the recipe today.
      int num_built = 0;
      if (recipe->days_per_unit <= 1) { // We can build it every day!
        num_built = workers;
        if (recipe->units_per_day > 0) {  // We can build multiples every day!
          num_built *= recipe->units_per_day;
        }
      } else {  // It takes multiple days to build a unit.
// Each worker reduces our counter by 1.
// We copy the counter to another variable so that if $real is false, we don't
// have to do any modifications; we can copy it back later
        int days_left = recipe_amt->days_until_built - workers;
        while (days_left <= 0) {
          num_built++;
          days_left += recipe->days_per_unit;
        }
        if (real) { // Copy the value back.
          recipe_amt->days_until_built = days_left;
        }
      } // recipe->days_per_unit > 1
// Make sure we won't build more than are enqueued.
      if (recipe_amt->amount != INFINITE_RESOURCE &&
          num_built > recipe_amt->amount) {
        num_built = recipe_amt->amount;
      }

/* Next, check how many units of the resource manufactured we have the component
 * resources/minerals for.
 * We copy the city's resources and minerals to a new array since if $real is
 * false, we don't want to actually deduct the city's resources.
 */
      int tmp_resources[RES_MAX];
      int tmp_minerals[MINERAL_MAX];
      for (int i = 0; i < RES_MAX; i++) {
        tmp_resources[i] = city->resources[i];
      }
      for (int i = 0; i < MINERAL_MAX; i++) {
        tmp_minerals[i] = city->minerals[i];
      }

/* We use a for loop since we might eventually run out of resources/minerals and
 * will need to stop before we've built num_built units.  We could use math to
 * determine the number of units we'll actually be able to build, but this seems
 * more clear.
 */
      bool can_build_more = true;
      for (int i = 0; can_build_more && i < num_built; i++) {
// Check that we have the resources and minerals before expending them, so that
// we don't expend one only to find that we lack the other.
        bool has_resources = true, has_minerals = true;
        for (int n = 0;
             has_resources && n < recipe->resource_ingredients.size();
             n++) {
          Resource_amount res_amt = recipe->resource_ingredients[n];
          if (tmp_resources[res_amt.type] < res_amt.amount) {
            has_resources = false;
          }
        }
        for (int n = 0;
             has_minerals && n < recipe->mineral_ingredients.size();
             n++) {
          Mineral_amount min_amt = recipe->mineral_ingredients[n];
          if (tmp_minerals[min_amt.type] < min_amt.amount) {
            has_minerals = false;
          }
        }

        if (has_resources && has_minerals &&
            city->has_resources(recipe->resource_ingredients) &&
            city->has_minerals (recipe->mineral_ingredients )) {

          resource_gains[recipe->result.type] += recipe->result.amount;

// Consume resources/minerals from our temp lists.
          for (int n = 0; n < recipe->resource_ingredients.size(); n++) {
            Resource_amount res_amt = recipe->resource_ingredients[n];
            tmp_resources[res_amt.type] -= res_amt.amount;
          }
          for (int n = 0; n < recipe->mineral_ingredients.size(); n++) {
            Mineral_amount min_amt = recipe->mineral_ingredients[n];
            tmp_minerals[min_amt.type] -= min_amt.amount;
          }

          if (real) {
// Actually consume resources/minerals.
            city->expend_resources(recipe->resource_ingredients);
            city->expend_minerals (recipe->mineral_ingredients );
            if (recipe_amt->amount != INFINITE_RESOURCE) {
              recipe_amt->amount--;
            }
          }

        } else { // Ran out of resources
          can_build_more = false;
        }
      } // for (int i = 0; can_build_more && i < num_built; i++)
// Check if we've built all that were enqueued.
      if (real &&
          recipe_amt->amount != INFINITE_RESOURCE && recipe_amt->amount <= 0) {
        build_queue.erase( build_queue.begin() );
      }
    } // if (!build_queue.empty())
  } // if (workers > 0)

  return resource_gains;
}

// real defaults to false
std::map<Mineral,int> Building::get_mineral_production(City* city, bool real)
{
// Sinceonly mines can produce minerals, we only return anything if we produce
// RES_MINING.
// TODO: If there is another way buildings can produce minerals, we need to
//       support that here!
  if (!open) { // Closed buildings don't produce anything!
    return std::map<Mineral,int>();
  }

  if (!city) {
    debugmsg("Building::get_mineral_production(NULL) called!");
    return std::map<Mineral,int>();
  }

  if (!produces_resource(RES_MINING)) {
    return std::map<Mineral,int>();
  }

  Player_city* pl_city = NULL;
  if (city->is_player_city()) {
    pl_city = static_cast<Player_city*>(city);
  }

  Map_tile* tile = city->map.get_tile(pos);

  std::map<Mineral,int> mineral_gains;

  for (int n = 0; n < minerals_mined.size(); n++) {
    Mineral_amount min_mined = minerals_mined[n];
    Mineral_datum* min_dat = Mineral_data[min_mined.type];
    if (min_mined.amount > 0) {
// Track the number of workers assigned to this mineral.  If we exhaust the
// map's supply of this mineral, we'll remove the mineral from minerals_mined
// and will fire the workers.
      int min_workers = min_mined.amount;
      min_mined.amount *= shaft_output;
      Mineral_amount* tile_min = tile->lookup_mineral(min_mined.type);
      bool exhausted = false; // Did we use up all of the mineral?

      if (!tile_min) {  // The mineral doesn't exist on the map tile!
        exhausted = true;

      } else {

        if (tile_min->amount == INFINITE_RESOURCE) {
          mineral_gains[min_mined.type] += min_mined.amount;

        } else if (tile_min->amount < min_mined.amount) {
// We exhausted the supply of the mineral!
          exhausted = true;
          mineral_gains[min_mined.type] += tile_min->amount;

        } else {  // Mineral is neither infinite nor exhausted
          mineral_gains[min_mined.type] += min_mined.amount;
          if (real) {
            tile_min->amount -= min_mined.amount;
          }
        }

        if (real && exhausted) {  // We used up the map's supply of min_mined!
          tile->remove_mineral(min_mined.type);
          if (pl_city) {
            pl_city->fire_citizens(CIT_PEASANT, min_workers, this);
            pl_city->add_message(MESSAGE_MAJOR,
                                 "Our mine has exhausted its supply of %s!",
                                 min_dat->name.c_str());
          }
          minerals_mined.erase( minerals_mined.begin() + n );
          n--;
        }
      } // if (tile_min)
    } // if (min_mined.amount > 0)
  } // for (int n = 0; n < minerals_mined.size(); n++)

  return mineral_gains;
}


void Building::do_production(City* city)
{
  if (!open) {
    return; // Closed buildings don't produce anything!
  }

  if (!city) {
    debugmsg("Building::do_production(NULL) called!");
    return;
  }

  Building_datum* datum = get_building_datum();

  if (!datum) {
    debugmsg("Encountered NULL datum in Building::do_production()!");
    return;
  }

  std::map<Resource,int> resource_gains = get_resource_production(city, true);
  std::map<Mineral, int> mineral_gains  = get_mineral_production (city, true);

// Add the resources/minerals we've gathered to the city!
  city->gain_resources(resource_gains);
  city->gain_minerals (mineral_gains );
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

int Building::get_total_wages()
{
  return (workers * get_building_datum()->wages);
}

int Building::get_destroy_cost()
{
  Building_datum* build_dat = get_building_datum();
  if (build_dat) {
    return build_dat->destroy_cost;
  }
  return 0;
} 

int Building::get_reopen_cost()
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

// help_links and for_area both default to false.
std::string Building_datum::get_short_description(bool help_links,
                                                  bool for_area)
{
  std::stringstream ret;

  if (!for_area) {
    ret << "<c=white>" << capitalize( name ) << "<c=/>" << std::endl;
    ret << "<c=magenta>Type: " <<
           capitalize( building_category_name(category) ) << "<c=/>" <<
           std::endl;
  }

  ret << "<c=yellow>Build time: " << build_time << " days.<c=/>" << std::endl;
  if (!build_costs.empty()) {
    ret << "<c=yellow>Build cost: ";
    for (int i = 0; i < build_costs.size(); i++) {
      std::string res_name = Resource_data[ build_costs[i].type ]->name;

      if (help_links) {
        ret << "<link=" << res_name << ">";
      }

      ret << Resource_data[ build_costs[i].type ]->name;

      if (help_links) {
        ret << "</link><c=yellow>";
      }

      ret << " x " << build_costs[i].amount;

      if (i + 1 < build_costs.size()) {
        ret << ", ";
      }
    }
    ret << "<c=/>" << std::endl;
  }

  if (!for_area && unlockable) {
    if (help_links) {
      ret << "<link=unlockables>Unlock Condition</link>: ";
    } else {
      ret << "<c=ltblue>Unlock Condition: ";
    }
    ret << "<c=white>" << unlock_condition.get_description() << "<c=/>" <<
           std::endl;
  }

  if (!maintenance_cost.empty()) {
    ret << "<c=ltred>Maintenance cost: ";
    for (int i = 0; i < maintenance_cost.size(); i++) {
      std::string res_name = Resource_data[ maintenance_cost[i].type ]->name;

      if (help_links) {
        ret << "<link=" << res_name << ">";
      }

      ret << res_name;

      if (help_links) {
        ret << "</link><c=ltred>";
      }

      ret << " x " << maintenance_cost[i].amount;

      if (i + 1 < maintenance_cost.size()) {
        ret << ", ";
      }
    }
    ret << "<c=/>" << std::endl;
  }

  if (!housing.empty()) {
    if (help_links) {
      ret << "<link=housing>Housing</link> <c=brown>for ";
    } else {
      ret << "<c=brown>Housing for ";
    }
    for (int i = 0; i < housing.size(); i++) {
      std::string cit_name = citizen_type_name(housing[i].type, true);
      ret << housing[i].amount << " ";

      if (help_links) {
        ret << "<link=" << cit_name << ">";
      }

      ret << cit_name;

      if (help_links) {
        ret << "</link><c=brown>";
      }

      if (i + 1 < housing.size()) {
        ret << ", ";
      }
      if (i + 2 == housing.size()) {
        ret << "and ";
      }

    }
    ret << "<c=/>" << std::endl;
  }

  if (jobs.amount > 0) {
    ret << "<c=cyan>Employs " << jobs.amount << " ";

    if (help_links) {
      ret << "<link=" << citizen_type_name(jobs.type, true) << ">";
    }

    ret << citizen_type_name(jobs.type, (jobs.amount > 1));

    if (help_links) {
      ret << "</link><c=cyan>";
    }

    ret << ".<c=/>" << std::endl;

    if (!production.empty()) {
      ret << "<c=ltgreen>Each worker produces: ";
      for (int i = 0; i < production.size(); i++) {
        std::string res_name = Resource_data[ production[i].type ]->name;

        if (help_links) {
          ret << "<link=" << res_name << ">";
        }

        ret << res_name;

        if (help_links) {
          ret << "</link><c=ltgreen>";
        }

        ret << " x " << production[i].amount;
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
      std::string res_name = Resource_data[ recipes[i].get_resource() ]->name;

      if (help_links) {
        ret << "<link=" << res_name << ">";
      }

      ret << res_name;

      if (help_links) {
        ret << "</link><c=ltblue>";
      }

      if (i + 1 < recipes.size()) {
        ret << ", ";
      }
    }
    ret << "<c=/>" << std::endl;
  }

  return ret.str();
}

std::string Building_datum::generate_help_text()
{
  std::stringstream ret;
  ret << get_short_description(true) << std::endl << description;

  return ret.str();
}

bool Building_datum::built_by(Race race)
{
  if (!race_can_build.empty()) {
    for (int i = 0; i < race_can_build.size(); i++) {
      if (race_can_build[i] == race) {
        return true;
      }
    }
    return false;
  }

  if (!race_forbidden.empty()) {
    for (int i = 0; i < race_forbidden.size(); i++) {
      if (race_forbidden[i] == race) {
        return false;
      }
    }
    return true;
  }

  return true;
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
             name.c_str(), Resource_data[type]->name.c_str());
    return false;
  } else if (amount == 0) {
    debugmsg("%s produces zero %s - this is invalid.",
             name.c_str(), Resource_data[type]->name.c_str());
    return false;
  } else if (amount < 0) {
    debugmsg("%s produces negative %s - this is invalid.",
             name.c_str(), Resource_data[type]->name.c_str());
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

std::vector<Building_type> get_true_building_types()
{
  std::vector<Building_type> ret;
  for (int i = BUILD_NULL + 1; i < BUILD_MAX; i++) {
    Building_datum* build_dat = Building_data[i];
    if (build_dat->category != BUILDCAT_NULL) {
      ret.push_back( Building_type(i) );
    }
  }
  return ret;
}
