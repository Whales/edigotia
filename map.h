#ifndef _MAP_H_
#define _MAP_H_

#include <vector>

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
}

enum Map_type
{
  MAP_NULL = 0,

  MAP_PLAINS,       // Mostly field and rocky
  MAP_WASTELAND,    // Mostly rocky and hills
  MAP_FOOTHILLS,    // Mostly hills and mountains
  MAP_BASIN,        // River, then fields and swamp
  MAP_MOUNTAINOUS,  // Almost all mountains
  MAP_COASTAL,      // Ocean along one side

class City_map
{
  City_map();
  ~City_map();

  std::vector< std::vector<Terrain_type> > tiles;
};

#endif
