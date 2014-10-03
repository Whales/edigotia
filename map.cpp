#include "map.h"
#include "rng.h"
#include "geometry.h" // For direction
#include "window.h" // For debugmsg
#include <sstream>

Map_tile::Map_tile()
{
  ter = TER_NULL;
}

Map_tile::~Map_tile()
{
}

std::string Map_tile::get_terrain_name()
{
  return Terrain_data[ter]->name;
}

std::string Map_tile::get_info()
{
  std::stringstream ret;

  ret << Terrain_data[ter]->name << std::endl;

  if (!crops.empty()) {
    ret << "Crops: " << get_crop_info() << std::endl;
    ret << "Farm Output: " << get_farmability() << "%%%%%%%%";
  }

  return ret.str();
}

std::string Map_tile::get_crop_info()
{
  if (crops.empty()) {
    return std::string();
  }
  std::stringstream ret;
  for (int i = 0; i < crops.size(); i++) {
    ret << Crop_data[ crops[i] ]->name;
    if (i < crops.size() - 1) {
      ret << " ";
    }
  }
  return ret.str();
}

int Map_tile::get_farmability()
{
  return Terrain_data[ter]->farm_percent;
}

City_map::City_map()
{
}

City_map::~City_map()
{
}

void City_map::generate(Map_type type,
                        std::vector<Crop> crops, std::vector<Mineral> minerals,
                        Direction coast,
                        Direction_full river_start, Direction_full river_end)
{
  int chance[TER_MAX];  // Relative chance for each terrain type to appear.
  int total_chance = 0;
  for (int i = 0; i < TER_MAX; i++) {
    chance[i] = 0;
  }
  switch (type) {
    case MAP_TUNDRA:
      chance[TER_TUNDRA]        = 100;
      chance[TER_ROCKY]         =   1;
      chance[TER_HILL]          =   5;
      chance[TER_ICY_HILL]      =   2;
      chance[TER_MOUNTAIN]      =   5;
      chance[TER_ICY_MOUNTAIN]  =   2;
      chance[TER_FOREST]        =   5;
      break;

    case MAP_PLAINS:
      chance[TER_FIELD]     = 75;
      chance[TER_ROCKY]     = 14;
      chance[TER_HILL]      =  8;
      chance[TER_MOUNTAIN]  =  2;
      chance[TER_FOREST]    =  9;
      chance[TER_SWAMP]     =  1;
      break;

    case MAP_WASTELAND:
      chance[TER_FIELD]     =  8;
      chance[TER_ROCKY]     = 60;
      chance[TER_HILL]      = 14;
      chance[TER_MOUNTAIN]  =  1;
      chance[TER_FOREST]    =  2;
      chance[TER_DESERT]    =  6;
      break;

    case MAP_FOREST:
      chance[TER_FIELD]     =  5;
      chance[TER_ROCKY]     =  4;
      chance[TER_HILL]      =  2;
      chance[TER_MOUNTAIN]  =  1;
      chance[TER_FOREST]    = 45;
      chance[TER_SWAMP]     =  6;
      break;

    case MAP_FOOTHILLS:
      chance[TER_FIELD]     =  8;
      chance[TER_ROCKY]     = 20;
      chance[TER_HILL]      = 50;
      chance[TER_MOUNTAIN]  =  8;
      chance[TER_FOREST]    =  4;
      chance[TER_DESERT]    =  1;
      break;

    case MAP_ICY_FOOTHILLS:
      chance[TER_TUNDRA]       =  60;
      chance[TER_ROCKY]        =  10;
      chance[TER_HILL]         =  40;
      chance[TER_ICY_HILL]     = 100;
      chance[TER_MOUNTAIN]     =   8;
      chance[TER_ICY_MOUNTAIN] =  22;
      chance[TER_FOREST]       =   1;
      break;

    case MAP_MOUNTAINOUS:
      chance[TER_FIELD]     =  1;
      chance[TER_ROCKY]     = 10;
      chance[TER_HILL]      = 24;
      chance[TER_MOUNTAIN]  = 60;
      chance[TER_FOREST]    =  3;
      chance[TER_DESERT]    =  1;
      break;

    case MAP_ICY_MOUNTAIN:
      chance[TER_TUNDRA]       =   7;
      chance[TER_ROCKY]        =   3;
      chance[TER_HILL]         =  30;
      chance[TER_ICY_HILL]     =  60;
      chance[TER_MOUNTAIN]     =  20;
      chance[TER_ICY_MOUNTAIN] = 200;
      break;

    case MAP_BASIN:
      chance[TER_FIELD]     = 80;
      chance[TER_ROCKY]     =  4;
      chance[TER_HILL]      =  2;
      chance[TER_FOREST]    =  9;
      chance[TER_SWAMP]     =  7;
// We'll stick in a river later
      break;

    case MAP_CANYON:
      chance[TER_FIELD]     =  2;
      chance[TER_ROCKY]     = 12;
      chance[TER_HILL]      = 24;
      chance[TER_MOUNTAIN]  = 60;
      chance[TER_FOREST]    =  8;
      chance[TER_SWAMP]     =  1;
// We'll stick in a river later
      break;

    case MAP_GLACIER:
      chance[TER_TUNDRA]       = 60;
      chance[TER_ROCKY]        = 10;
      chance[TER_HILL]         =  3;
      chance[TER_ICY_HILL]     =  3;
      chance[TER_MOUNTAIN]     =  3;
      chance[TER_ICY_MOUNTAIN] =  3;
      chance[TER_FOREST]       =  5;
// We'll stick in a glacier later
      break;

    case MAP_COASTAL:
      chance[TER_FIELD]     = 30;
      chance[TER_ROCKY]     = 10;
      chance[TER_HILL]      =  2;
      chance[TER_FOREST]    =  6;
      chance[TER_SWAMP]     =  5;
      break;

    case MAP_SWAMP:
      chance[TER_FIELD]     =  4;
      chance[TER_ROCKY]     =  6;
      chance[TER_HILL]      =  4;
      chance[TER_MOUNTAIN]  =  1;
      chance[TER_FOREST]    =  7;
      chance[TER_SWAMP]     = 40;
      break;

    case MAP_DESERT:
      chance[TER_FIELD]     =  1;
      chance[TER_ROCKY]     = 15;
      chance[TER_HILL]      =  2;
      chance[TER_MOUNTAIN]  =  4;
      chance[TER_DESERT]    = 60;
      break;

    case MAP_OCEAN:
      chance[TER_FIELD]     =  1;
      chance[TER_MOUNTAIN]  =  1;
      chance[TER_DESERT]    =  2;
      chance[TER_OCEAN]     = 80;
      break;

    case MAP_ICECAP:
      chance[TER_ICE]       = 40;
      chance[TER_TUNDRA]    =  5;
      chance[TER_FOREST]    =  1;
  }

  if (type != MAP_OCEAN && coast != DIR_NULL) {  // Alter chances a bit.
    chance[TER_TUNDRA]    *= 1.5;
    chance[TER_FIELD]     *= 1.5;
    chance[TER_ROCKY]     *=  .8;
    chance[TER_HILL]      *=  .6;
    chance[TER_MOUNTAIN]  *=  .2;
    chance[TER_FOREST]    *= 1.3;
    chance[TER_SWAMP]     *= 1.2;
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
      tiles[x][y].ter = Terrain_type(index);
    }
  }

// Now fix up coastal / basin
  if (type != MAP_OCEAN && coast != DIR_NULL) {
    int x = 0, x2 = 1, x3 = 2, x4 = 3;
    int y = 0, y2 = 1, y3 = 2, y4 = 3;
    if (coast == DIR_SOUTH) {
      y  = CITY_MAP_SIZE - 1;
      y2 = CITY_MAP_SIZE - 2;
      y3 = CITY_MAP_SIZE - 3;
      y4 = CITY_MAP_SIZE - 4;
    } else if (coast == DIR_EAST) {
      x  = CITY_MAP_SIZE - 1;
      x2 = CITY_MAP_SIZE - 2;
      x3 = CITY_MAP_SIZE - 3;
      x4 = CITY_MAP_SIZE - 4;
    }
    for (int i = 0; i < CITY_MAP_SIZE; i++) {
      if (coast == DIR_NORTH || coast == DIR_SOUTH) {
        x = i;
        tiles[x][y2].ter = TER_OCEAN;
        if (one_in(5)) {
          tiles[x][y3].ter = TER_OCEAN;
          tiles[x][y4].ter = TER_OCEAN;
        } else if (one_in(3)) {
          tiles[x][y3].ter = TER_OCEAN;
        }
      } else {
        y = i;
        tiles[x2][y].ter = TER_OCEAN;
        if (one_in(5)) {
          tiles[x3][y].ter = TER_OCEAN;
          tiles[x4][y].ter = TER_OCEAN;
        } if (one_in(3)) {
          tiles[x3][y].ter = TER_OCEAN;
        }
      }
      tiles[x][y].ter = TER_OCEAN;
    }

  }
  if (Map_type_data[type]->is_river) {
// TODO: Do we need to specify which direction the river travels?
//       For now, it's just northwest => southeast
    Terrain_type river_type = TER_RIVER;
    if (type == MAP_GLACIER) {
      river_type = TER_GLACIER;
    }
    int x = 0, y = 0;
    int dx = 1, dy = 1;
    int mid = CITY_MAP_SIZE / 2, end = CITY_MAP_SIZE - 1;
    bool lake = false;
    switch (river_start) {
      case DIRFULL_NORTHWEST: x = 0;    y = 0;    break;
      case DIRFULL_WEST:      x = 0;    y = mid;  break;
      case DIRFULL_NORTH:     x = mid;  y = 0;    break;
      case DIRFULL_SOUTHWEST: x = 0;    y = end;  break;
      case DIRFULL_NORTHEAST: x = end;  y = 0;    break;

/* Special case!  There's a river coming in but nowhere for it to go... that
 * makes this a LAKE!  COOOOOOL.  So stick a blob of lake somewhere and send our
 * river towards it.
 */
      case DIRFULL_NULL:
        lake = true;
// We need to set x/y based on river_end, and treat that as the start.
        switch (river_end) {
          case DIRFULL_SOUTHEAST: x = end;  y = end;  break;
          case DIRFULL_SOUTH:     x = mid;  y = end;  break;
          case DIRFULL_EAST:      x = end;  y = mid;  break;
          case DIRFULL_SOUTHWEST: x =   0;  y = end;  break;
          case DIRFULL_NORTHEAST: x = end;  y =   0;  break;

// Not a lake after all! :(  Should never happen.
          case DIRFULL_NULL:
            lake = false;
            break;
        }
        break;
    }

// Figure out what direction to move in based on where the river should end.
    switch (river_end) {
      case DIRFULL_SOUTHEAST: dx =  1;  dy =  1;  break;
      case DIRFULL_SOUTH:     dx =  0;  dy =  1;  break;
      case DIRFULL_EAST:      dx =  1;  dy =  0;  break;
      case DIRFULL_SOUTHWEST: dx = -1;  dy =  1;  break;
      case DIRFULL_NORTHEAST: dx =  1;  dy = -1;  break;

/* Special case!  There's a river coming in but nowhere for it to go... that
 * makes this a LAKE!  COOOOOOL.  So stick a blob of lake somewhere and send our
 * river towards it.
 */
      case DIRFULL_NULL:
        lake = true;
        break;
    }

// If x or y were set to the middle of the side, we can actually randomize their
// starting position a little bit.
    if (x == mid) {
      x += rng(0 - CITY_MAP_SIZE / 3, CITY_MAP_SIZE / 3);
    }
    if (y == mid) {
      y += rng(0 - CITY_MAP_SIZE / 3, CITY_MAP_SIZE / 3);
    }

    if (lake) {
      int lake_x = rng(1, CITY_MAP_SIZE - 3),
          lake_y = rng(1, CITY_MAP_SIZE - 3);
// Store the center tile in a buffer just in case the lake overwrites it
      Terrain_type buffer = tiles[CITY_MAP_SIZE / 2][CITY_MAP_SIZE / 2].ter;
      tiles[lake_x    ][lake_y    ].ter = TER_LAKE;
      tiles[lake_x + 1][lake_y    ].ter = TER_LAKE;
      tiles[lake_x    ][lake_y + 1].ter = TER_LAKE;
      tiles[lake_x + 1][lake_y + 1].ter = TER_LAKE;
      tiles[CITY_MAP_SIZE / 2][CITY_MAP_SIZE / 2].ter = buffer;
      while (x < lake_x || x > lake_x + 1 || y < lake_y || y > lake_y + 1) {
        if (tiles[x][y].ter != TER_OCEAN) {
          tiles[x][y].ter = river_type;
        }
        if (one_in(2)) {
          if (x < lake_x) {
            x++;
          } else if (x > lake_x + 1) {
            x--;
          }
        } else {
          if (y < lake_y) {
            y++;
          } else if (y > lake_y + 1) {
            y--;
          }
        }
      }

    } else { // Not a lake BOOOOO GIMME A LAKE

      while (x >= 0 && y >= 0 && x < CITY_MAP_SIZE && y < CITY_MAP_SIZE) {
// Don't replace ocean with river!
        if (tiles[x][y].ter != TER_OCEAN) {
          tiles[x][y].ter = river_type;
        }
        if (one_in(2)) {
          if (dx == 0) {
            if (x == 0) {
              x++;
            } else if (x == CITY_MAP_SIZE - 1) {
              x--;
            } else {  // Random move
              x += (one_in(2) ? 1 : -1);
            }
          } else {
            x += dx;
          }
        } else {
          if (dy == 0) {
            if (y == 0) {
              y++;
            } else if (y == CITY_MAP_SIZE - 1) {
              y--;
            } else {  // Random move
              y += (one_in(2) ? 1 : -1);
            }
          } else {
            y += dy;
          }
        } // End of "move x or y?" block
      } // while (x >= 0 && y >= 0 && x < CITY_MAP_SIZE && y < CITY_MAP_SIZE)
    } // (!lake)

  } else if (type == MAP_OCEAN) {
// Obviously, not every ocean tile is going to have a prominent center isle;
// however since this is specifically for cities on ocean tiles, let's always
// give an island.
// TODO: Except if we have mermen, don't do this?
    for (int x = CITY_MAP_SIZE / 2 - 2; x <= CITY_MAP_SIZE / 2 + 2; x++) {
      for (int y = CITY_MAP_SIZE / 2 - 2; y <= CITY_MAP_SIZE / 2 + 2; y++) {
        if (one_in(4)) {
          tiles[x][y].ter = TER_DESERT;
        } else if (one_in(5)) {
          tiles[x][y].ter = TER_ROCKY;
        } else if ((x == CITY_MAP_SIZE / 2 && y == CITY_MAP_SIZE / 2) ||
                   one_in(2)) {
          tiles[x][y].ter = TER_FIELD;
        }
      }
    }
  }

// Now place some resources...
  for (int x = 0; x < CITY_MAP_SIZE; x++) {
    for (int y = 0; y < CITY_MAP_SIZE; y++) {
/* For safety's sake - sometimes if we examine a map, decide not to build a city
 * there, then return to the same map, this becomes relevent because the old
 * resources are still there from the first generation.
 */
      tiles[x][y].crops.clear();
      tiles[x][y].minerals.clear();
      Terrain_datum* ter_dat = Terrain_data[ tiles[x][y].ter ];
      for (int i = 0; i < ter_dat->crops.size(); i++) {
// Check if the world map assigned us this crop.
        Crop crop = ter_dat->crops[i];
        bool crop_assigned = false;
        for (int n = 0; !crop_assigned && n < crops.size(); n++) {
          if (crops[n] == crop) {
            crop_assigned = true;
          }
        }
// Only assign the crop if we got it from the world map, or on a small chance
        Crop_datum* crop_dat = Crop_data[crop];
        if (!crop_dat) {
          debugmsg("NULL crop_dat (crop %d)", crop);
        }
        if (crop_assigned ||
            (rng(1, 100) <= crop_dat->percentage &&
             rng(1, 100) <= crop_dat->percentage &&
             rng(1, 100) <= crop_dat->percentage   )) {
          tiles[x][y].crops.push_back(crop);
        }
      }
      for (int i = 0; i < ter_dat->minerals.size(); i++) {
        Mineral_amount min_amount = ter_dat->minerals[i];
        Mineral mineral = min_amount.type;
// Check if the world map assigned us this mineral.
        bool mineral_assigned = false;
        for (int n = 0; !mineral_assigned && n < minerals.size(); n++) {
          if (minerals[n] == mineral) {
            mineral_assigned = true;
          }
        }
        Mineral_datum* mineral_dat = Mineral_data[mineral];
        if (!mineral_dat) {
          debugmsg("NULL mineral_dat (mineral %d)", mineral);
        }
// Some terrains have an infinite amount of a mineral (e.g. hills & stone)
// In this case, we leave it unchanged (obviously) and always assign it (obvs)
        if (min_amount.is_infinite()) {
          tiles[x][y].minerals.push_back( min_amount );
        } else if (mineral_assigned) {
          tiles[x][y].minerals.push_back( min_amount.randomize() );
        } else if (rng(1, 150) <= mineral_dat->percentage) {
          tiles[x][y].minerals.push_back( min_amount.make_small() );
        }
      }
    }
  }
}

Map_tile* City_map::get_tile(Point p)
{
  return get_tile(p.x, p.y);
}

Map_tile* City_map::get_tile(int x, int y)
{
  if (is_oob(x, y)) {
    return NULL;
  }
  return &(tiles[x][y]);
}

std::string City_map::get_resource_info(Point p)
{
  return get_resource_info(p.x, p.y);
}

std::string City_map::get_resource_info(int x, int y)
{
  if (is_oob(x, y)) {
    return std::string();
  }

  std::stringstream ret;
  return ret.str();
}

Terrain_datum* City_map::get_terrain_datum(Point p)
{
  return get_terrain_datum(p.x, p.y);
}

Terrain_datum* City_map::get_terrain_datum(int x, int y)
{
  if (is_oob(x, y)) {
    return Terrain_data[TER_NULL]; // Better/safer than returning NULL
  }
  return Terrain_data[ tiles[x][y].ter ];
}

std::string City_map::get_terrain_name(Point p)
{
  return get_terrain_name(p.x, p.y);
}

std::string City_map::get_terrain_name(int x, int y)
{
  if (is_oob(x, y)) {
    return "Out of bounds";
  }
  return tiles[x][y].get_terrain_name();
}

glyph City_map::get_glyph(Point p)
{
  return get_glyph(p.x, p.y);
}

glyph City_map::get_glyph(int x, int y)
{
  if (is_oob(x, y)) {
    return glyph();
  }

  Terrain_type ter = tiles[x][y].ter;
  Terrain_datum* terdata = Terrain_data[ter];
  return terdata->symbol;
}

std::string City_map::get_info(Point p)
{
  return get_info(p.x, p.y);
}

std::string City_map::get_info(int x, int y)
{
  if (is_oob(x, y)) {
    std::stringstream ret;
    ret << "Out of Bounds [" << x << ":" << y << "]";
    return ret.str();
  }

  return tiles[x][y].get_info();
}

int City_map::get_farmability(Point p)
{
  return get_farmability(p.x, p.y);
}

int City_map::get_farmability(int x, int y)
{
  if (is_oob(x, y)) {
    return 0;
  }
  return tiles[x][y].get_farmability();
}

bool City_map::is_oob(int x, int y)
{
  return (x < 0 || x >= CITY_MAP_SIZE || y < 0 || y >= CITY_MAP_SIZE);
}
