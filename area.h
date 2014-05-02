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
#include <string>

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
};

struct Area
{
  Area_type type;
  Point pos;
};

extern Area_datum* Area_data[AREA_MAX];
void init_area_data();

#endif
