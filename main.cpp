#include "city.h"
#include "rng.h"
#include "window.h"
#include "world.h"
#include "interface.h"
#include "game.h"

int main()
{
// seed the RNG.  TODO: wrap this
  srand(time(NULL));

// Start up our display (e.g. ncurses)
  init_display();

// Init the game data!
  init_building_data();
  init_area_data();
  init_terrain_data();
  init_map_type_data();

// Generate a world...
  World_map world;
  world.generate();
// ... for now, we always start by picking a location for a new city.
  Point p = world.draw();

  if (p.x == -1) {
    return 0;
  }

// Put our city there.
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

// Set up a game.
  Game game;
  game.init();

// Set up an interface bound to our city and game, and kick off its loop.
  Interface interface;
  interface.init(&game, &city);
  interface.main_loop();

  endwin();
  return 0;
}

