#ifndef _BUILDING_H_
#define _BUILDING_H_

#include "citizen.h"
#include "resource.h"
#include "geometry.h"
#include "city_achievement.h" // For unlockable buildings.
#include "animal.h"
#include "race.h" // For buildings with race restrictions.
#include <string>
#include <vector>
#include <map>
#include <istream>

class City; // For use in Building::amount_built()

enum Building_type
{
  BUILD_NULL = 0,

// Infrastructure - ALL AREAS, not real buildings
  BUILD_PARK,
  BUILD_PLAZA,
  BUILD_MARKETPLACE,

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
  BUILD_SMELTERY,
  BUILD_MINT,
  BUILD_KITCHEN,
  BUILD_TAILOR,
  BUILD_WOOD_SHOP,
  BUILD_SMITH,

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

// These are defined in building.cpp
Building_category lookup_building_category(std::string name);
std::string building_category_name(Building_category category);

// get_true_building_types() returns all Building_types that are NOT just data
// for an area (i.e. Building_category != BUILDCAT_NULL).  It's defined in
// building.cpp.
std::vector<Building_type> get_true_building_types();

struct Recipe
{
// R and A combine to form a Resource_amount.
  Recipe(Resource R = RES_NULL, int A = 1);

  std::string save_data();
  bool load_data(std::istream& data);

  std::string get_name(); // Returns name, or the result name if name is empty
  Resource get_resource();  // Returns result.type()

  std::string name;

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

// max_deficit is used for AI and defaults to -10.  Basically, an AI_city won't
// add this building if it has a deficit of more than this value of any of the
// required resources or minerals.
  int max_deficit;

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
// If help_links is true, then the return string has all references to other
// data surrounded by <link=...> </link> tags.
  std::string get_short_description(bool help_links = false,
                                    bool for_area = false);
  std::string generate_help_text(); // Generates text with all critical data

  bool built_by(Race race);

  bool produces_resource(Resource res = RES_NULL);
  bool builds_resource  (Resource res = RES_NULL);
  int  amount_produced  (Resource res);
  int  get_total_jobs   (Citizen_type cit_type = CIT_NULL);
  int  get_housing      (Citizen_type cit_type = CIT_NULL);

// Setup functions
  bool add_production(Resource type, int amount);

  int uid;  // Unique identifier; not sure if we need this but doesn't hurt

  std::string name;
  std::string description;  // Formatted for help; see help.h
  bool plural;  // If true, name is treated as a plural

  Building_category category;

  int build_time;
  std::vector<Resource_amount> build_costs;
// race_forbidden is a list of races that can NOT build this.  All others CAN.
// race_can_build is a list of races that CAN build this.  All others can NOT.
// If both are empty, all races can build this.
  std::vector<Race> race_forbidden;
  std::vector<Race> race_can_build;

  bool unlockable;  // If false, this building is available from the start.
  City_achievement unlock_condition;

  int destroy_cost; // How much gold it costs to destroy this

  std::vector<Citizen_amount> housing;
  int military_support;
  int livestock_space;
  int base_morale;  // A morale bonus to all citizens just for existing

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

  std::string save_data();
  bool load_data(std::istream& data);

  void set_type(Building_type new_type);
  void make_queued(); // Prepare it for being in a building queue
  void close(City* city);

  Building_datum* get_building_datum();
  std::string get_name();
/* get_resource_production() includes all resources produced or built!  This
 * includes food/resources produced by crops, if we are a farm.  Obviously it
 * does not include minerals and crops produced by mines and farms.  It also
 * does not include any resources we may get from hunting animals, since that's
 * (semi) random.  It does not include any meta-resources, like RES_FARMING;
 * only resources that the meta-resource will result in.
 * If $real is set to true, then we'll deduct wood from the map, actually use up
 * resources to build stuff, etc.
 */
  std::map<Resource,int> get_resource_production(City* city, bool real = false);
// Same idea, but only works on mines obviously.
  std::map<Mineral, int> get_mineral_production (City* city, bool real = false);

/* do_production() calls get_resource_production() and get_mineral_production()
 * with real = true and adds the return values to the city.
 */
  void do_production(City* city);

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
  int get_total_wages();
  int get_destroy_cost();
  int get_reopen_cost();
  std::map<Resource,int> get_maintenance();

// ALL-BUILDINGS DATA
  Building_type type;
  bool open;  // If false, we can't hire workers, pay no maintenance, etc etc
  Point pos;  // Position of the area we belong to, if any.
  int construction_left;  // Days of construction remaining.

  int workers;

// HOUSING ONLY
  int get_housing(Citizen_type cit_type = CIT_NULL);

// FARMS ONLY
  int get_empty_fields();
  int field_output; // How much of each crop does a single field output?

  std::vector<Crop_amount> crops_grown;

// MINES ONLY
  int get_empty_shafts();
// If we have any minerals with amount HIDDEN_RESOURCE attempt to discover them
  void discover_minerals(City* city);
  int shaft_output; // How much of each mineral does a single shaft output?

  std::vector<Mineral_amount> minerals_mined;

// HUNTING CAMPS ONLY
  int hunter_level; // Combat level of hunters; compare to animals' danger value
  Animal hunting_target;  // The animal we're currently hunting
  Animal_action hunting_action; // What we do with animals we hunt
  int get_max_hunt_prey();  // The number of animals we can catch (total)
  int get_max_hunt_food();  // get_max_hunt_prey() * hunting_target->food_killed

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
