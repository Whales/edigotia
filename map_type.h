#ifndef _MAP_TYPE_H_
#define _MAP_TYPE_H_

/* This header is included to avoid circular dependencies.
 * The original cause: Race_datum (race.h) requires the enum Map_type, which was
 * originally in map.h.  However, Map_type_datum (map.h) requires City_role from
 * ai.h; and ai.h requires Race_skill from race.h.  Since all of these are enums
 * we cannot resolve this dependency without creating a new header just for enum
 * Map_type.
 */

// Map_type defines what your City_map looks like.  It's also used as the basic
// tile for World_map.
enum Map_type
{
  MAP_NULL = 0,

  MAP_TUNDRA,       // Mostly tundra, some forest

  MAP_DESERT,       // Mostly desert
  MAP_WASTELAND,    // Mostly rocky and hills
  MAP_PLAINS,       // Mostly field and rocky
  MAP_FOREST,       // Mostly forest and plains
  MAP_SWAMP,        // Mostly swamp
  MAP_JUNGLE,       // Jungle, forest and swamp.

  MAP_FOOTHILLS,    // Mostly hills and mountains
  MAP_ICY_FOOTHILLS,// Frozen hills and mountains
  MAP_MOUNTAINOUS,  // Almost all mountains
  MAP_ICY_MOUNTAIN, // The most forbidding of all terrains!

  MAP_BASIN,        // River, then fields and swamp
  MAP_CANYON,       // River, then mountains and hills
  MAP_GLACIER,      // Frozen river

  MAP_COASTAL,      // Ocean along one side
  MAP_OCEAN,        // Almost all ocean, occasional island
  MAP_ICECAP,       // Frozen ocean

  MAP_MAX
};

#endif
