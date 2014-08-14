#include "city.h"
#include "rng.h"
#include "window.h"
#include "world.h"
#include "interface.h"

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
  Point p = world.draw();

  if (p.x == -1) {
    return 0;
  }

  City city;
  city.map.generate( world.tiles[p.x][p.y], world.coast_from(p.x, p.y));
  bool placed = false;
  while (!placed) {
    placed = city.place_keep();
    if (!placed) {
      p = world.draw();
      city.map.generate( world.tiles[p.x][p.y], world.coast_from(p.x, p.y));
    }
  }

  Interface interface;
  interface.init(&city);
  interface.main_loop();

  endwin();
  return 0;
}

