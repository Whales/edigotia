#ifndef _WORLD_H_
#define _WORLD_H_

#include "map.h"
#include "geometry.h"
#include "window.h"
#include "resource.h"
#include <vector>
#include <map>

#define WORLD_MAP_SIZE 500

class World_map
{
public:
  World_map();
  ~World_map();

  void generate();

  Point draw(Window* w_map = NULL);

  Direction coast_from(int x, int y);
  std::vector<Crop>    crops_at   (int x, int y);
  std::vector<Mineral> minerals_at(int x, int y);
  std::vector<Crop>    crops_at   (Point p);
  std::vector<Mineral> minerals_at(Point p);

  Map_type tiles                [WORLD_MAP_SIZE][WORLD_MAP_SIZE];
  int altitude                  [WORLD_MAP_SIZE][WORLD_MAP_SIZE];
  int rainfall                  [WORLD_MAP_SIZE][WORLD_MAP_SIZE];
  int temperature               [WORLD_MAP_SIZE][WORLD_MAP_SIZE];
  int continent_id              [WORLD_MAP_SIZE][WORLD_MAP_SIZE];
  bool river                    [WORLD_MAP_SIZE][WORLD_MAP_SIZE];
  int crops                     [WORLD_MAP_SIZE][WORLD_MAP_SIZE];
  int minerals                  [WORLD_MAP_SIZE][WORLD_MAP_SIZE];

  std::vector<Point> continents;
  std::map<int, std::vector<int> > joined_continents;

private:
  void add_continent(Point origin, int height = 100, int step = 8, int id = 0);
  void add_river    (Point origin);
  void add_crop     (Point origin, Crop crop,       int radius);
  void add_mineral  (Point origin, Mineral mineral, int radius);
/* add_crop() and add_mineral() are both basically the same function, so rather
 * than rewrite it, add_resource() does both.  If crop != CROP_NULL it will
 * place crop, otherwise it will place mineral.  If both are null (or neither
 * are), it will return without doing anything.
 */
  void add_resource (Point origin, Crop crop, Mineral mineral, int radius);

};

#endif
