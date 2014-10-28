#ifndef _KINGDOM_H_
#define _KINGDOM_H_

#include "city.h"
#include "color.h"
#include "race.h"
#include "world.h"
#include <vector>

#define STANDARD_KINGDOM_SIZE 80

class Kingdom
{
public:
  Kingdom();
  ~Kingdom();

  bool place_in_world(World_map* world, int size = STANDARD_KINGDOM_SIZE);

// Data
  int uid;
  Race race;
  City capital;
  nc_color color;

  std::vector<City> dukes;
};

// See kingdom.cpp
extern std::vector<Kingdom*> Kingdoms;
void init_kingdoms(World_map* world);

#endif
