#ifndef _WORLD_H_
#define _WORLD_H_

#include "map.h"
#include "geometry.h"
#include "window.h"
#include <vector>
#include <map>

#define WORLD_MAP_SIZE 500

class World_map
{
public:
  World_map();
  ~World_map();

  void generate();
  void add_continent(Point origin, int height = 100, int step = 8, int id = 0);
  void add_river(Point origin);

  Point draw(Window* w_map = NULL);

  Direction coast_from(int x, int y);

  Map_type tiles[WORLD_MAP_SIZE][WORLD_MAP_SIZE];
  int altitude[WORLD_MAP_SIZE][WORLD_MAP_SIZE];
  int rainfall[WORLD_MAP_SIZE][WORLD_MAP_SIZE];
  int temperature[WORLD_MAP_SIZE][WORLD_MAP_SIZE];
  int continent_id[WORLD_MAP_SIZE][WORLD_MAP_SIZE];
  bool river[WORLD_MAP_SIZE][WORLD_MAP_SIZE];

  std::vector<Point> continents;
  std::map<int, std::vector<int> > joined_continents;
};

#endif
