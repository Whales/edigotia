#include "city.h"
#include "rng.h"
#include "window.h"

int main()
{
  srand(time(NULL));

  init_display();

  init_building_data();
  init_area_data();
  init_terrain_data();

  City city;

  for (int i = 1; i < MAP_MAX; i++) {
    city.map.generate(Map_type(i));
    city.place_keep();
  }

  endwin();
  return 0;
}

