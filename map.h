#ifndef _MAP_H_
#define _MAP_H_

#include <vector>
#include "geometry.h" // For direction

#define CITY_MAP_SIZE 8

enum Terrain_type
{
  TER_NULL = 0,

  TER_FIELD,      // Good for farming
  TER_ROCKY,      // OK for farming, good for quarry, OK for mining
  TER_HILL,       // Good for mining, OK for quarry
  TER_MOUNTAIN,   // Great for mining, OK for quarry

  TER_RIVER,      // Good for fishing
  TER_OCEAN,      // Food for fishing

  TER_DESERT,     // Bad for most everything
  TER_SWAMP,      // Bad for most everything

  TER_MAX
};

enum Map_type
{
  MAP_NULL = 0,

  MAP_PLAINS,       // Mostly field and rocky
  MAP_WASTELAND,    // Mostly rocky and hills
  MAP_FOOTHILLS,    // Mostly hills and mountains
  MAP_BASIN,        // River, then fields and swamp
  MAP_MOUNTAINOUS,  // Almost all mountains
  MAP_COASTAL,      // Ocean along one side
  MAP_SWAMP,        // Mostly swamp
  MAP_DESERT,       // Mostly desert

  MAP_MAX
};

class City_map
{
  City_map();
  ~City_map();

  void generate(Map_type type, Direction coast = DIR_NULL);

  Terrain_type tiles[CITY_MAP_SIZE][CITY_MAP_SIZE];
};

#endif
