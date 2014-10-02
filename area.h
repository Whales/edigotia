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
  AREA_QUARRY,
  AREA_MINE,

// Military
  AREA_BARRACKS,

  AREA_MAX
};

struct Area_datum
{
  std::string name;
  int uid;
  glyph symbol;

  Building_type building;

  Building_datum* get_building_datum();
};

struct Area
{
  Area(Area_type T = AREA_NULL, Point P = Point(-1, -1));

  void make_queued(); // Set our construction_left, etc.

  Area_datum* get_area_datum();
  Building_type get_building_type();
  Building_datum* get_building_datum();

  std::map<Resource,int> get_maintenance();

// DATA
  Area_type type;

/* If open is false, this area is inactive and is treated as though it doesn't
 * exist for all purposes.  When we enqueue an area to our building queue, open
 * is set to false, and construction_left is set to its type construction time.
 * Each day, the area at the top of the queue has its construction_left
 * decreased by an appropriate amount (usually 1); once it hits 0, it's removed
 * from the construction queue, open is set to true, and it's added to the
 * City's area vector.
 */
  bool open;
  int construction_left;

// Our position on the City_map.
  Point pos;

// Since areas function as buildings too, we create a Building item to track
// everything a building does.
  Building building;
};

extern Area_datum* Area_data[AREA_MAX];
void init_area_data();
Building_datum* get_building_for(Area_type area);

#endif
