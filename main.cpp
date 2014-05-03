#include "city.h"
#include "rng.h"
#include "window.h"
#include "world.h"

int main()
{
  srand(time(NULL));

  init_display();

  init_building_data();
  init_area_data();
  init_terrain_data();
  init_map_type_data();

  World_map world;
  world.generate();
  world.draw();

  City city;

  for (int i = 1; i < MAP_MAX; i++) {
    city.map.generate(Map_type(i));
    city.place_keep();
  }

  endwin();
  return 0;
}

