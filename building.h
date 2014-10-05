#ifndef _BUILDING_H_
#define _BUILDING_H_

#include <string>
#include <vector>
#include "citizen.h"
#include "resource.h"

enum Building_type
{
  BUILD_NULL = 0,

// Housing
  BUILD_HOVEL,
  BUILD_HOUSE,
  BUILD_MANOR,
  BUILD_KEEP,

// Basic resources
  BUILD_FARM,
  BUILD_QUARRY,
  BUILD_MINE,
  BUILD_CAMP,
  BUILD_SAWMILL,

// Military
  BUILD_BARRACKS,

  BUILD_MAX
};

struct Building_datum
{
  Building_datum();
  ~Building_datum();

// Data functions
  std::string get_short_description();
  bool produces_resource(Resource res);
  int get_total_jobs    (Citizen_type cit_type = CIT_NULL);

// Setup functions
  bool add_production(Resource type, int amount);

  int uid;
  std::string name;
  bool plural;  // If true, name is treated as a plural
  std::vector<Resource_amount> build_costs;
  int upkeep;

  std::vector<Citizen_amount> housing;
  int military_support;

  Citizen_amount jobs;
  int wages;  // Measured in 1/10th of a gold!

  int build_time;
  std::vector<Resource_amount> production; // Per job, w/ skill of 5 (max)
  std::vector<Resource_amount> maintenance_cost;

// Things this building can construct
  std::vector<Resource> buildable;

};

struct Building
{
  Building();
  ~Building();

  void set_type(Building_type new_type);

  Building_datum* get_building_datum();
  bool produces_resource(Resource res);

  int get_total_jobs    (Citizen_type cit_type = CIT_NULL);
  int get_available_jobs(Citizen_type cit_type = CIT_NULL);
  int get_filled_jobs   (Citizen_type cit_type = CIT_NULL);

  Building_type type;
  int workers;

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
  std::vector<Resource_amount> build_queue;

// Basically for farms only
  int get_empty_fields();
  int field_output; // How much of each crop does a single field output?
  std::vector<Crop_amount> crops_grown;
// Basically for mines only
  int get_empty_shafts();
  int shaft_output; // How much of each mineral does a single shaft output?
  std::vector<Mineral_amount> minerals_mined;

};

extern Building_datum* Building_data[BUILD_MAX];
void init_building_data();

#endif
