#ifndef _MAP_H_
#define _MAP_H_

#include <vector>
#include "geometry.h" // For direction
#include "glyph.h"    // For terrain glyph

#define CITY_MAP_SIZE 9

enum Terrain_type
{
  TER_NULL = 0,

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
// TODO: List of crops available
// TODO: List of minables
// TODO: List of wildlife
};

extern Terrain_datum* Terrain_data[TER_MAX];
void init_terrain_data();

enum Map_type
{
  MAP_NULL = 0,

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

  MAP_MAX
};

class City_map
{
public:
  City_map();
  ~City_map();

  void generate(Map_type type, Direction coast = DIR_NULL);

  Terrain_type tiles[CITY_MAP_SIZE][CITY_MAP_SIZE];
};

#endif
