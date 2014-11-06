#ifndef _AREA_H_
#define _AREA_H_

/* Areas are similar to buildings, except they're placed on the city map.  As
 * such, since their placement is very limited (and dependent on city growth),
 * areas should a) be very important, and b) support a lot of citizens.
 * Examples include living areas, farms, mines, etc.
 */

#include "glyph.h"
#include "building.h"
#include "geometry.h"
#include "resource.h"
#include <string>
#include <map>

class City;
class Player_city;

enum Area_type
{
  AREA_NULL = 0,

// Housing
  AREA_HOVELS,
  AREA_HOUSES,
  AREA_MANOR,
  AREA_KEEP,  // Always the center tile; always start with this

// Raw resource production
  AREA_FARM,
  AREA_HUNTING_CAMP,
  AREA_MINE,
  AREA_SAWMILL,

// Military
  AREA_BARRACKS,

  AREA_MAX
};

// NOTE: There should be no more than 9 buildings in each category!
//       If a category is full, split it into two categories.

enum Area_category
{
  AREACAT_NULL = 0,

  AREACAT_HOUSING,  // Things that provide housing - hovels, houses, manor, keep

  AREACAT_FOOD,     // Things that produce food - farm, hunting camp, fishery
  AREACAT_RESOURCES,// Things that produce non-food resources - mine, sawmill

  AREACAT_MILITARY, // Any military structure - barracks

  AREACAT_MAX
};

Area_category lookup_area_category(std::string name);
std::string area_category_name(Area_category category);

struct Area_datum
{
  std::string name;
  int uid;
  glyph symbol;

  Area_category category;

  Building_type building;

  Building_datum* get_building_datum();
};

struct Area
{
  Area(Area_type T = AREA_NULL, Point P = Point(-1, -1));

  void make_queued(); // Set our construction_left, etc.
  void close(City* city); // Fire all workers, set open to false, etc.
// Automatically hire workers and choose crops/minerals
  void auto_hire(Player_city* city);

  bool under_construction();  // True if building.construction_left > 0

  Area_datum* get_area_datum();
  std::string get_name();
  Building_type get_building_type();
  Building_datum* get_building_datum();

  int get_destroy_cost();
  int get_reopen_cost();
  std::map<Resource,int> get_maintenance();

  bool produces_resource(Resource res);
  int  amount_produced  (Resource res);

// DATA
  Area_type type;

/* If open is false, this area is inactive and is treated as though it doesn't
 * exist for all purposes.  When we enqueue an area to our building queue, open
 * is set to false, and building's construction_left is set to its type's
 * construction time.
 * Each day, the area at the top of the queue has its building's
 * construction_left decreased by an appropriate amount (usually 1); once it
 * hits 0, it's removed from the construction queue, open is set to true, and
 * it's added to the City's area vector.
 */
  bool open;

// Our position on the City_map.
  Point pos;

// Since areas function as buildings too, we create a Building item to track
// everything a building does.
  Building building;
};

// Defined in area_data.cpp
extern Area_datum* Area_data[AREA_MAX];
void init_area_data();

Building_datum* get_building_for(Area_type area);

#endif
