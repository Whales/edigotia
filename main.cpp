#include "city.h"
#include "rng.h"
#include "window.h"
#include "world.h"
#include "interface.h"
#include "game.h"
#include "init.h"

void check_world_resources(World_map* world);

int main()
{
// seed the RNG.  TODO: wrap this
  srand(time(NULL));

// Start up our display (e.g. ncurses)
  init_display();

// Init the game data!
  if (!init_all_data()) {
    debugmsg("Failed to init data!");
    return 1;
  }

// Generate a world...
  World_map world;
  world.generate();
// Tool for debugging / testing the amount of resources in the world.
  //check_world_resources(&world);
// ... for now, we always start by picking a location for a new city.
  Point p = world.draw();

  if (p.x == -1) {  // We hit ESC
    return 0;
  }

// Put our city there.
  City city;
  std::vector<Crop>    crops    = world.crops_at   (p);
  std::vector<Mineral> minerals = world.minerals_at(p);
  city.map.generate( world.get_map_type(p), crops, minerals,
                     world.coast_from(p) );
  bool placed = false;
  while (!placed) {
    placed = city.place_keep();
    if (!placed) {
      p = world.draw();
      crops    = world.crops_at   (p);
      minerals = world.minerals_at(p);
      city.map.generate( world.get_map_type(p), crops, minerals,
                         world.coast_from(p) );
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

void check_world_resources(World_map* world)
{
  int land = world->land_count();
  int size = WORLD_MAP_SIZE * WORLD_MAP_SIZE;
  float land_percent = float((100.0 * float(land)) / size);
  debugmsg("Land tiles: %d/%d (%f percent)", land, size, land_percent);
  for (int i = 0; i < CROP_MAX; i++) {
    Crop crop = Crop(i);
    int crop_count = world->crop_count(crop);
    float crop_percent = float((100.0 * float(crop_count)) / size);
    float crop_land_percent = float((100.0 * float(crop_count)) / land);
    debugmsg("%s: %d/%d/%d (%f/%f percent, should be %d)",
             Crop_data[crop]->name.c_str(), crop_count, land, size,
             crop_land_percent, crop_percent, Crop_data[crop]->percentage);
  }
  for (int i = 0; i < MINERAL_MAX; i++) {
    Mineral mineral = Mineral(i);
    int mineral_count = world->mineral_count(mineral);
    float mineral_percent = float((100.0 * float(mineral_count)) / size);
    float mineral_land_percent = float((100.0 * float(mineral_count)) / land);
    debugmsg("%s: %d/%d/%d (%f/%f percent, should be %d)",
             Mineral_data[mineral]->name.c_str(), mineral_count, land, size,
             mineral_land_percent, mineral_percent,
             Mineral_data[mineral]->percentage);
  }
}
