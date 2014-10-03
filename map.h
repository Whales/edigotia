#ifndef _MAP_H_
#define _MAP_H_

#include "geometry.h" // For direction
#include "glyph.h"    // For terrain glyph
#include "resource.h" // Crops and minerals
#include "area.h"
#include <vector>
#include <string>

#define CITY_MAP_SIZE 9

enum Terrain_type
{
  TER_NULL = 0,

  TER_TUNDRA,     // Not good for much - some oil

  TER_FIELD,      // Good for farming
  TER_ROCKY,      // OK for farming, good for quarry, OK for mining
  TER_HILL,       // Good for mining, OK for quarry
  TER_MOUNTAIN,   // Great for mining, OK for quarry

  TER_FOREST,     // Great for hunting, abundant wood

  TER_RIVER,      // Good for fishing
  TER_OCEAN,      // Good for fishing

  TER_DESERT,     // Bad for most everything
  TER_SWAMP,      // Bad for most everything

  TER_MAX
};

struct Terrain_datum
{
  std::string name;
  glyph symbol;
  int farm_percent;
  std::vector<Crop> crops;
  std::vector<Mineral_amount> minerals;
// TODO: List of wildlife

  std::vector<Area_type> buildable_areas;
};

extern Terrain_datum* Terrain_data[TER_MAX];
void init_terrain_data();

enum Map_type
{
  MAP_NULL = 0,

  MAP_TUNDRA,       // Mostly tundra, some forest

  MAP_DESERT,       // Mostly desert
  MAP_WASTELAND,    // Mostly rocky and hills
  MAP_PLAINS,       // Mostly field and rocky
  MAP_FOREST,       // Mostly forest and plains
  MAP_SWAMP,        // Mostly swamp

  MAP_FOOTHILLS,    // Mostly hills and mountains
  MAP_MOUNTAINOUS,  // Almost all mountains

  MAP_BASIN,        // River, then fields and swamp
  MAP_CANYON,       // River, then mountains and hills

  MAP_COASTAL,      // Ocean along one side
  MAP_OCEAN,        // Almost all ocean, occasional island
  MAP_ICECAP,       // Frozen ocean

  MAP_MAX
};

struct Map_type_datum
{
  std::string name;
  glyph symbol;
};

extern Map_type_datum* Map_type_data[MAP_MAX];

void init_map_type_data();

struct Map_tile
{
  Map_tile();
  ~Map_tile();

  Terrain_type                ter;
  std::vector<Crop>           crops;
  std::vector<Mineral_amount> minerals;

  std::string get_terrain_name();
  std::string get_info();
  std::string get_crop_info();

  int get_farmability();
};

class City_map
{
public:
  City_map();
  ~City_map();

/* crops and minerals are derived from the world map, and are a list of the
 * resources that are plentiful here and should be placed in all eligable tiles.
 * Other resources may be present, but they should not necessarily appear in all
 * tiles, and they should appear in smaller amounts.
 */
  void generate(Map_type type,
                std::vector<Crop> crops, std::vector<Mineral> minerals,
                Direction coast = DIR_NULL);

  Map_tile* get_tile(Point p);
  Map_tile* get_tile(int x, int y);

  std::string get_resource_info(Point p);
  std::string get_resource_info(int x, int y);

  Terrain_datum* get_terrain_datum(Point p);
  Terrain_datum* get_terrain_datum(int x, int y);

  std::string get_terrain_name(Point p);
  std::string get_terrain_name(int x, int y);

  glyph get_glyph(Point p);
  glyph get_glyph(int x, int y);

  std::string get_info(Point p);
  std::string get_info(int x, int y);

  int get_farmability(Point p);
  int get_farmability(int x, int y);

/*
  Terrain_type          tiles   [CITY_MAP_SIZE][CITY_MAP_SIZE];
  std::vector<Crop>     crops   [CITY_MAP_SIZE][CITY_MAP_SIZE];
  std::vector<Mineral>  minerals[CITY_MAP_SIZE][CITY_MAP_SIZE] ;
*/

private:

  Map_tile tiles[CITY_MAP_SIZE][CITY_MAP_SIZE];

/* These are derived from the world map; they're the resources that we have
 * notable/large amounts of.  Other resources (particularly minerals) may be
 * available, but only in small quantities if at all.
 */
  std::vector<Crop>    bonus_crops;
  std::vector<Mineral> bonus_minerals;

  bool is_oob(int x, int y);

};

#endif
