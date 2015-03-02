#ifndef _WORLD_H_
#define _WORLD_H_

#include "map.h"
#include "geometry.h"
#include "window.h"
#include "resource.h"
#include "pathfind.h" // We store several Generic_maps for different kinds of A*
#include <vector>
#include <map>
#include <string>
#include <istream>

#define WORLD_MAP_SIZE 400

// For A* pathfinding
enum A_star_status
{
  A_STAR_NONE,
  A_STAR_OPEN,
  A_STAR_CLOSED
};

// A record of which tiles have been seen
class Map_seen
{
public:
  Map_seen();
  ~Map_seen();

  std::string save_data();
  bool load_data(std::istream& data);

  bool is_seen  (Point p);
  bool is_seen  (int x, int y);

  void mark_seen(Point p);
  void mark_seen(int x, int y);

private:
  bool seen[WORLD_MAP_SIZE][WORLD_MAP_SIZE];
};

class World_map
{
public:
  World_map();
  ~World_map();

  void generate();

  bool save_to_file  (std::string filename);
  bool load_from_file(std::string filename);

// Interactive; returns the point the player hits "enter" on, or (-1, -1) if
// they hit Esc or Q
  Point draw(Point start = Point(-1, -1), Map_seen* seen = NULL);

  std::string get_name();

  int land_count(); // Returns the number of tiles that are not ocean
/* crop_count() and mineral_count() return the number of tiles with the
 * specified crop/mineral.  If it's omitted (or {CROP,MINERAL}_NULL is passed)
 * then they return the number of tiles with ANY crop/mineral.
 */
  int crop_count    (Crop crop = CROP_NULL);
  int mineral_count (Mineral mineral = MINERAL_NULL);
  int animal_count  (Animal animal = ANIMAL_NULL);

  Map_type get_map_type(Point p);
  Map_type get_map_type(int x, int y);

  void set_kingdom_id(Point p, int id);
  void set_kingdom_id(int x, int y, int id);
  int  get_kingdom_id(Point p);
  int  get_kingdom_id(int x, int y);

  void  set_city(Point p, City* new_city);
  void  set_city(int x, int y, City* new_city);
  City* get_city(Point p);
  City* get_city(int x, int y);
  City* lookup_city_uid(int uid);

// Just references road[][]
  bool has_road(Point p);
  bool has_road(int x, int y);
// Returns Map_type_data[ get_map_type(p) ]->road_cost; for purposes of building
  int road_cost(Point p);
  int road_cost(int x, int y);
// Returns Map_type_data[ cur_map_type(p) ]->travel_cost; if traveler is NOT
// RACE_NULL, we check its Race_datum to see if it overrides the travel_cost.
  int travel_cost(Point p, Race traveler = RACE_NULL);
  int travel_cost(int x, int y, Race traveler = RACE_NULL);
// Returns the travel_cost() for the shortest route from start to end for the
// specified race.
  int route_cost(Race traveler, Point start, Point end);
  int route_cost(Race traveler, int x0, int y0, int x1, int y1);
// Looks at adjacent tiles to decide which line drawing glyph to use
  glyph get_road_glyph(Point p);
  glyph get_road_glyph(int x, int y);
// true if the Map_datum has is_river == true
  bool is_river(Point p);
  bool is_river(int x, int y);
// true if the Map_datum has is_water == true
  bool is_water(Point p);
  bool is_water(int x, int y);

  Direction coast_from     (Point p);
  Direction coast_from     (int x, int y);
/* river_{start,end}_for() tells us what direction the river starts and ends for
 * the given point - like coast_from(), this is for City_map generation.
 * river_start_for() looks northwest, west/north, northeast, southwest for a
 * river (in that order).
 * river_end_for() looks southeast, south/east, southwest, northeast for a river
 * or ocean (in that order).
 * Obviously they should never return the same value.  I made river_start_for()
 * check northwest before southwest because we DON'T want randomization between
 * those two - we can only guarantee that these two functions won't return the
 * same value if the order the overlapping tiles are checked in is NOT random.
 * (Also because I think in real life rivers are more likely to run from the
 * northeast than from the southwest.)
 */
  Direction_full river_start_for(Point p);
  Direction_full river_start_for(int x, int y);
  Direction_full river_end_for  (Point p);
  Direction_full river_end_for  (int x, int y);

  std::vector<Point> get_path(int x0, int y0, int x1, int y1);
  std::vector<Point> get_path(Point start, Point end);

  int get_trade_distance(Race trader, int x0, int y0, int x1, int y1);
  int get_trade_distance(Race trader, Point start, Point end);

  bool build_road(int x0, int y0, int x1, int y1);
  bool build_road(Point start, Point end);

  std::vector<Crop>    crops_at   (Point p);
  std::vector<Mineral> minerals_at(Point p);
  std::vector<Animal>  animals_at (Point p);
  std::vector<Crop>    crops_at   (int x, int y);
  std::vector<Mineral> minerals_at(int x, int y);
  std::vector<Animal>  animals_at (int x, int y);

  bool has_crop   (Crop crop,       Point p);
  bool has_mineral(Mineral mineral, Point p);
  bool has_animal (Animal animal,   Point p);
  bool has_crop   (Crop crop,       int x, int y);
  bool has_mineral(Mineral mineral, int x, int y);
  bool has_animal (Animal animal,   int x, int y);

  std::vector<City*> city_list;

private:
  void set_random_name();

  void add_continent(Point origin, int height = 100, int step = 8, int id = 0);
  void add_river    (Point origin);
  void add_crop     (Point origin, Crop crop,       int radius);
  void add_mineral  (Point origin, Mineral mineral, int radius);
  void add_animal   (Point origin, Animal animal,   int radius);
/* add_crop(), add_mineral() and add_animal() are all basically the same
 * function, so rather than rewrite it three times, add_resource() does them
 * all.  Exactly one of {crop, mineral, animal} should be non-null; the other
 * two should be null.  The non-null one will be placed.  If this requirement is
 * not met, the function will return without doing anything.
 */
  void add_resource (Point origin, Crop crop, Mineral mineral, Animal animal,
                     int radius);

  void update_road_map();
// If RACE_NULL is used (or no race is passed), then we update the maps for ALL
// races.  Can be time consuming!
  void update_travel_map(Race traveler = RACE_NULL);

  bool tile_okay_for_crop(Point p, Crop crop);
  bool tile_okay_for_crop(int x, int y, Crop crop);

  bool tile_okay_for_animal(Point p, Animal animal);
  bool tile_okay_for_animal(int x, int y, Animal animal);

// DATA

  std::string name;

  Map_type tiles  [WORLD_MAP_SIZE][WORLD_MAP_SIZE];
  int altitude    [WORLD_MAP_SIZE][WORLD_MAP_SIZE];
  int rainfall    [WORLD_MAP_SIZE][WORLD_MAP_SIZE];
  int temperature [WORLD_MAP_SIZE][WORLD_MAP_SIZE];
  int continent_id[WORLD_MAP_SIZE][WORLD_MAP_SIZE];
  int kingdom_id  [WORLD_MAP_SIZE][WORLD_MAP_SIZE];
  City* city      [WORLD_MAP_SIZE][WORLD_MAP_SIZE];
  bool river      [WORLD_MAP_SIZE][WORLD_MAP_SIZE];
  bool road       [WORLD_MAP_SIZE][WORLD_MAP_SIZE];
// crops and minerals are bitmaps that indicate what's here
  int crops       [WORLD_MAP_SIZE][WORLD_MAP_SIZE];
  int minerals    [WORLD_MAP_SIZE][WORLD_MAP_SIZE];
  int animals     [WORLD_MAP_SIZE][WORLD_MAP_SIZE];

// road_map is a map of the road_costs of various terrain.  We will need to
// update it whenever new roads are built or terrain changes.
  Generic_map road_map;

/* travel_map is a map of the travel_cost of various terrain, modified for the
 * presence of roads, rivers, etc.  It will need to be updated whenever new
 * roads are built or terrain changes.
 * We store one for each race, because each race travels on terrain differently;
 * for instance, elves can travel quite easily through forests, whereas humans
 * cannot.
 */
  std::map<Race,Generic_map> travel_map;

  std::vector<Point> continents;
  std::map<int, std::vector<int> > joined_continents;

};

#endif
