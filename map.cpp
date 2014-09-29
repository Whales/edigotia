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

std::string Map_tile::get_info()
{
  std::stringstream ret;

  ret << Terrain_data[ter]->name << std::endl;

  if (!crops.empty()) {
    ret << "Crops:";
    for (int i = 0; i < crops.size(); i++) {
      ret << " " << Crop_data[ crops[i] ]->name;
    }
    ret << std::endl << "Farm Output: " << Terrain_data[ter]->farm_percent <<
           "%%%%%%%%";
  }

  return ret.str();
}

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
    case MAP_TUNDRA:
      chance[TER_TUNDRA]    = 60;
      chance[TER_ROCKY]     =  1;
      chance[TER_HILL]      =  3;
      chance[TER_MOUNTAIN]  =  3;
      chance[TER_FOREST]    =  5;
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
      chance[TER_DESERT]    =  3;
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
    case MAP_MOUNTAINOUS:
      chance[TER_FIELD]     =  1;
      chance[TER_ROCKY]     = 10;
      chance[TER_HILL]      = 24;
      chance[TER_MOUNTAIN]  = 60;
      chance[TER_FOREST]    =  3;
      chance[TER_DESERT]    =  1;
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

  } else if (type == MAP_BASIN || type == MAP_CANYON) {
// TODO: Do we need to specify which direction the river travels?
//       For now, it's just northwest => southeast
    int x = 0, y = 0;
    while (x < CITY_MAP_SIZE && y < CITY_MAP_SIZE) {
      tiles[x][y].ter = TER_RIVER;
      if (one_in(2)) {
        x++;
      } else {
        y++;
      }
    }
  } else if (type == MAP_OCEAN){
// Obviously, not every ocean tile is going to have a prominent center isle;
// however since this is specifically for cities on ocean tiles, let's always
// give an island.
// TODO: Except if we have mermen, don't do this?
    for (int x = CITY_MAP_SIZE / 2 - 2; x <= CITY_MAP_SIZE / 2 +2; x++) {
      for (int y = CITY_MAP_SIZE / 2 - 2; y <= CITY_MAP_SIZE / 2 +2; y++) {
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
      Terrain_datum* ter_dat = Terrain_data[ tiles[x][y].ter ];
      for (int i = 0; i < ter_dat->crops.size(); i++) {
        Crop_datum* crop_dat = Crop_data[ ter_dat->crops[i] ];
        if (!crop_dat) {
          debugmsg("NULL crop_dat (crop %d)", ter_dat->crops[i]);
        }
        if (rng(1, 100) <= crop_dat->percentage) {
          tiles[x][y].crops.push_back( ter_dat->crops[i] );
        }
      }
      for (int i = 0; i < ter_dat->minerals.size(); i++) {
        Mineral_datum* mineral_dat = Mineral_data[ ter_dat->minerals[i].type ];
        if (rng(1, 100) <= mineral_dat->percentage) {
          tiles[x][y].minerals.push_back( ter_dat->minerals[i].randomize() );
        }
      }
    }
  }
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

bool City_map::is_oob(int x, int y)
{
  return (x < 0 || x >= CITY_MAP_SIZE || y < 0 || y >= CITY_MAP_SIZE);
}
