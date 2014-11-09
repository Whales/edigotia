#ifndef _BUILDING_H_
#define _BUILDING_H_

#include "citizen.h"
#include "resource.h"
#include "geometry.h"
#include "city_achievement.h" // For unlockable buildings.
#include <string>
#include <vector>
#include <map>

class City; // For use in Building::amount_built()

enum Building_type
{
  BUILD_NULL = 0,

// Housing - ALL AREAS, not real buildings
  BUILD_HOVEL,
  BUILD_HOUSE,
  BUILD_MANOR,
  BUILD_KEEP,

// Basic resources - ALL AREAS, not real buildings
  BUILD_FARM,
  BUILD_HUNTING_CAMP,
  BUILD_MINE,
  BUILD_SAWMILL,
  BUILD_PASTURE,

// Military - ALL AREAS, not real buildings
  BUILD_BARRACKS,

// Manufacturing
  BUILD_MASONRY,
  BUILD_MINT,

  BUILD_MAX
};

// NOTE: There should be no more than 9 buildings in each category!
//       If a category is full, split it into two categories.

enum Building_category
{
  BUILDCAT_NULL = 0,  // Used for buildings that only appear as areas

  BUILDCAT_MANUFACTURING, // Buildings that produce resources from raw materials

/* TODO: (just some rough ideas/plans)
 * Social (tavern, colluseum, theater, gardens, circus)
 * Religious (shrine, temple, cathedral, idol, ziggurat)
 * Financial (Market, bank)
 * Government (Embassy, courthouse, guardhouse)
 * Medical (apothecary, hospital)
 * Magic (wizard's tower, alchemy lab)
 * Educational (school, library, university, observatory)
 * Decorational (gardens, park, monument)
 */

  BUILDCAT_MAX
};

Building_category lookup_building_category(std::string name);
std::string building_category_name(Building_category category);

struct Recipe
{
// R and A combine to form a Resource_amount.
  Recipe(Resource R = RES_NULL, int A = 1);

  Resource get_resource();  // Returns result.type()

  Resource_amount result;

/* Obviously, only one of the following two should be set.  In
 * building_data.cpp, we use macros that set one variable and set the other to
 * 0.  When producing recipes, we check for whichever is not zero and use that.
 * Also, both are multiplied by the number of workers; 2 units per day actually
 * produces 6 if we have 3 workers, and 3 days per unit is actually 1 day per
 * unit if we have 3 workers.
 */
  int units_per_day;  // Defaults to 1
  int days_per_unit;  // Defaults to 0

  std::vector<Resource_amount> resource_ingredients;
  std::vector<Mineral_amount>  mineral_ingredients;
};

// For the building's queue
struct Recipe_amount
{
  Recipe_amount(Recipe R = Recipe(), int A = 1) : recipe (R), amount (A)
  { days_until_built = 0; }

  Resource get_resource();  // Returns recipe.get_resource()

  Recipe recipe;
  int amount;
  int days_until_built;

};

struct Building_datum
{
  Building_datum();
  ~Building_datum();

// Data functions
  std::string get_short_description();
  bool produces_resource(Resource res = RES_NULL);
  bool builds_resource  (Resource res = RES_NULL);
  int  amount_produced  (Resource res);
  int  get_total_jobs   (Citizen_type cit_type = CIT_NULL);
  int  get_housing      (Citizen_type cit_type = CIT_NULL);

// Setup functions
  bool add_production(Resource type, int amount);

  int uid;  // Unique identifier; not sure if we need this but doesn't hurt

  std::string name;
  bool plural;  // If true, name is treated as a plural

  std::string description;

  Building_category category;

  int build_time;
  std::vector<Resource_amount> build_costs;

  bool unlockable;  // If false, this building is available from the start.
  City_achievement unlock_condition;

  int destroy_cost; // How much gold it costs to destroy this

  std::vector<Citizen_amount> housing;
  int military_support;
  int livestock_space;

  Citizen_amount jobs;
  int wages;  // Measured in 1/10th of a gold!

  std::vector<Resource_amount> production; // Per job, w/ skill of 5 (max)

  int upkeep; // Daily cost in gold, measured in 1/10th of a gold
  std::vector<Resource_amount> maintenance_cost;  // Will we ever need this?

// Things this building can construct
  std::vector<Recipe>   recipes;

};

struct Building
{
  Building();
  ~Building();

  void set_type(Building_type new_type);
  void make_queued(); // Prepare it for being in a building queue

  Building_datum* get_building_datum();
  std::string get_name();
  bool produces_resource(Resource res = RES_NULL);
  bool builds_resource  (Resource res = RES_NULL);
  int  amount_produced  (Resource res);
  int  amount_built     (Resource res, City* city);
  int  livestock_space  ();

  int get_total_jobs    (Citizen_type cit_type = CIT_NULL);
  int get_available_jobs(Citizen_type cit_type = CIT_NULL);
  int get_filled_jobs   (Citizen_type cit_type = CIT_NULL);
  Citizen_type get_job_citizen_type();

  int get_upkeep();
  std::map<Resource,int> get_maintenance();

// ALL-BUILDINGS DATA
  Point pos;  // Position of the area we belong to, if any.
  int construction_left;  // Days of construction remaining.

  Building_type type;
  int workers;

// HOUSING ONLY
  int get_housing(Citizen_type cit_type = CIT_NULL);

// FARMS ONLY
  int get_empty_fields();
  int field_output; // How much of each crop does a single field output?

  std::vector<Crop_amount> crops_grown;

// MINES ONLY
  int get_empty_shafts();
  int shaft_output; // How much of each mineral does a single shaft output?

  std::vector<Mineral_amount> minerals_mined;

// HUNTING CAMPS ONLY
  int hunter_level; // Combat level of hunter; compare to animals' danger value

/* build_queue is a list of things this building will produce; when the building
 * is not currently building anything, it looks at the first item in the queue
 * and starts building that.  Once it finishes, it decreases that item's amount
 * by 1 (unless the amount is infinite), removes the item if the amount is now
 * 0, and looks for a new item to build.  If we do not have the resources to
 * build the first item in the queue, we look at the second and so on.  If we do
 * not have the resources to build ANY items in the queue (and the queue is not
 * empty), we generate an alert telling the player that nothing is buildable.
 * If the queue is empty but the building has workers, we generate an alert
 * telling the player that the building is idle.
 */
  std::vector<Recipe_amount> build_queue;

};

// Defined in building_data.cpp
extern Building_datum* Building_data[BUILD_MAX];
void init_building_data();

#endif
