#include "map.h"
#include "rng.h"
#include "geometry.h" // For direction

City_map::City_map()
{
}

City_map::~City_map()
{
}

void City_map::generate(Map_type type, Direction coast)
{
  int chance[TER_MAX];
  int total_chance = 0;
  for (int i = 0; i < TER_MAX; i++) {
    chance[i] = 0;
  }
  switch (type) {
    case MAP_PLAINS:
      chance[TER_FIELD]     = 15;
      chance[TER_ROCKY]     =  7;
      chance[TER_HILL]      =  4;
      chance[TER_MOUNTAIN]  =  1;
      chance[TER_SWAMP]     =  1;
      break;
    case MAP_WASTELAND:
      chance[TER_FIELD]     =  4;
      chance[TER_ROCKY]     = 10;
      chance[TER_HILL]      =  8;
      chance[TER_MOUNTAIN]  =  2;
      chance[TER_DESERT]    =  2;
      break;
    case MAP_FOOTHILLS:
      chance[TER_FIELD]     =  5;
      chance[TER_ROCKY]     =  8;
      chance[TER_HILL]      = 15;
      chance[TER_MOUNTAIN]  =  5;
      chance[TER_DESERT]    =  1;
      break;
    case MAP_BASIN:
      chance[TER_FIELD]     = 10;
      chance[TER_ROCKY]     =  1;
      chance[TER_HILL]      =  1;
      chance[TER_SWAMP]     =  4;
// We'll stick in a river later
      break;
    case MAP_MOUNTAINOUS:
      chance[TER_FIELD]     =  1;
      chance[TER_ROCKY]     =  5;
      chance[TER_HILL]      =  8;
      chance[TER_MOUNTAIN]  = 15;
      chance[TER_DESERT]    =  1;
      break;
    case MAP_COASTAL:
      chance[TER_FIELD]     = 15;
      chance[TER_ROCKY]     =  4;
      chance[TER_HILL]      =  1;
      chance[TER_SWAMP]     =  5;
      break;
    case MAP_SWAMP:
      chance[TER_FIELD]     =  4;
      chance[TER_ROCKY]     =  6;
      chance[TER_HILL]      =  3;
      chance[TER_MOUNTAIN]  =  1;
      chance[TER_SWAMP]     = 20;
      break;
    case MAP_DESERT:
      chance[TER_FIELD]     =  1;
      chance[TER_ROCKY]     =  5;
      chance[TER_HILL]      =  1;
      chance[TER_MOUNTAIN]  =  3;
      chance[TER_DESERT]    = 15;
      break;
  }

  for (int i = 0; i < TER_MAX; i++) {
    total_chance += chance[i];
  }

  for (int x = 0; x < CITY_MAP_SIZE; x++) {
    for (int y = 0; y < CITY_MAP_SIZE; y++) {
      int sel = rng(1, total_chance);
      int index = 0;
      while (sel > 0) {
        sel -= chance[index];
        if (sel > 0) {
          index++;
        }
      }
      tiles[x][y] = Terrain_type(index);
    }
  }

// Now fix up coastal / basin
  if (type == MAP_COASTAL) {
    int x = 0, x2 = 1;
    int y = 0, y2 = 1;
    if (coast == DIR_SOUTH) {
      y  = CITY_MAP_SIZE - 1;
      y2 = CITY_MAP_SIZE - 2;
    } else if (coast == DIR_EAST) {
      x  = CITY_MAP_SIZE - 1;
      x2 = CITY_MAP_SIZE - 2;
    }
    for (int i = 0; i < CITY_MAP_SIZE; i++) {
      if (coast == DIR_NORTH || coast == DIR_SOUTH) {
        x = i;
        if (one_in(4)) {
          tiles[x][y2] = TER_OCEAN;
        }
      } else {
        y = i;
        if (one_in(4)) {
          tiles[x2][y] = TER_OCEAN;
        }
      }
      tiles[x][y] = TER_OCEAN;
    }

  } else if (type == MAP_BASIN) {
// TODO: Do we need to specify which direction the river travels?
//       For now, it's just northwest => southeast
    int x = 0, y = 0;
    while (x < CITY_MAP_SIZE && y < CITY_MAP_SIZE) {
      tiles[x][y] = TER_RIVER;
      if (one_in(2)) {
        x++;
      } else {
        y++;
      }
    }
  }
}
