#ifndef _WORLD_H_
#define _WORLD_H_

#include "map.h"
#include "geometry.h"

#define WORLD_MAP_SIZE 500

class World_map
{
public:
  World_map();
  ~World_map();

  void generate();
  void add_continent(Point origin, int step = 8);
  void add_river(Point origin);

  void draw();

  Map_type tiles[WORLD_MAP_SIZE][WORLD_MAP_SIZE];
  int altitude[WORLD_MAP_SIZE][WORLD_MAP_SIZE];
  int rainfall[WORLD_MAP_SIZE][WORLD_MAP_SIZE];
  bool river[WORLD_MAP_SIZE][WORLD_MAP_SIZE];
};

#endif
