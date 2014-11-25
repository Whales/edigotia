#ifndef _MAP_H_
#define _MAP_H_

#include "geometry.h" // For direction
#include "glyph.h"    // For terrain glyph
#include "resource.h" // Crops and minerals
#include "area.h"
#include "animal.h"
#include "ai.h"       // For City_role
#include <vector>
#include <string>

// How big the city map is to a side.
#define CITY_MAP_SIZE 9

// Terrain_type is the basic tile for City_map and is linked to a Terrain_datum
enum Terrain_type
{
  TER_NULL = 0,

  TER_TUNDRA,     // Not good for much - some coal
  TER_ICY_HILL,   // Frozen hill, good for mining & that's all
  TER_ICY_MOUNTAIN, // Bitterly cold mountain, near useless!
  TER_GLACIER,    // Frozen river, useless

  TER_FIELD,      // Good for farming
  TER_ROCKY,      // OK for farming, OK for mining
  TER_HILL,       // Good for mining
  TER_MOUNTAIN,   // Great for mining

  TER_FOREST,     // Great for hunting, abundant wood
  TER_BARREN,     // Appears when forest is cleared; OK for farming

  TER_RIVER,      // Good for fishing
  TER_LAKE,       // Very good for fishing
  TER_OCEAN,      // Great for fishing

  TER_DESERT,     // Bad for most everything, but some unique crops
  TER_SWAMP,      // Bad for most everything, but some unique crops
  TER_JUNGLE,     // Great for hunting, abundant wood

  TER_ICE,        // From icecap - nearly useless

  TER_MAX
};

struct Terrain_datum
{
  Terrain_datum()
  {
    name = "Unknown";
    symbol = glyph('x', c_red, c_white);
    farm_percent = 0;
    wood_min = 0;
    wood_max = 0;
    wood_cleared_type = TER_NULL;
  };

  std::string name;
  glyph symbol;

  int farm_percent;
// This value is treated as a "null animal" when hunting; we pick what animal we
// hunt by random choice, each animal weighted by its population.  So if there
// are 10 hares, 6 foxes, and this value is 4, there's a 10-in-20 chance of
// hunting a hare, 6-in-20 of a fox, and 4-in-20 of nothing.
// Reasonable value range is 100 to 1000.  Use higher values for terrain that's
// easy to hide in (e.g. forest)
  int hunting_difficulty;
  int wood_min, wood_max;

  Terrain_type wood_cleared_type; // What to become when wood is cleared out.

  std::vector<Crop> crops;
  std::vector<Mineral_amount> minerals;
  std::vector<Animal_amount> animals;

  std::vector<Area_type> buildable_areas;
};

// Defined in map_data.cpp
extern Terrain_datum* Terrain_data[TER_MAX];
void init_terrain_data();

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

struct Map_type_datum
{
  Map_type_datum()
  {
    name = "NULL"; is_water = false; is_river = false;
    symbol = glyph('x', c_red, c_ltgray);
  }

  std::string name;
  bool is_water;
  bool is_river;
  glyph symbol;

  std::vector<City_role> city_roles;

};

// Defined in map_type_data.cpp
extern Map_type_datum* Map_type_data[MAP_MAX];
void init_map_type_data();

struct Map_tile
{
  Map_tile();
  ~Map_tile();

  Terrain_datum* get_terrain_datum();
  std::string    get_terrain_name();
  glyph          get_glyph();
  std::string    get_info();
  std::string    get_crop_info();
  std::string    get_animals_info();

  int get_farmability();
  int get_max_food_output();  // Pick the best crop and multiply by farmability

  int get_mineral_amount(Mineral mineral);

  Animal choose_hunt_animal(int skill_level);

  void clear_wood();  // Become whatever we become when wood is cleared out.

  Terrain_type ter;
  int wood;
  std::vector<Crop>           crops;
  std::vector<Mineral_amount> minerals;
  std::vector<Animal_amount>  animals;
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
                std::vector<Animal> animals,
                Direction coast = DIR_NULL,
                Direction_full river_start = DIRFULL_NULL,
                Direction_full river_end = DIRFULL_NULL);

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
