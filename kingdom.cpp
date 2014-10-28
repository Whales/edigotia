#include "kingdom.h"
#include "rng.h"
#include "window.h" // For debugmsg()
#include "geometry.h" // For rl_dist()

std::vector<Kingdom*> Kingdoms;

void init_kingdoms(World_map* world)
{
  if (!world) {
    debugmsg("init_kingdoms() called with NULL world!");
    return;
  }

  bool color_free[c_null];  // c_null is the last color
  for (int i = 0; i < c_null; i++) {
    color_free[i] = true;
  }
// One kingdom for each race.  Start at 1 to skip RACE_NULL.
  for (int i = 1; i < RACE_MAX; i++) {
    Kingdom* kingdom = new Kingdom;
    kingdom->uid = i;
    kingdom->race = Race(i);
    Race_datum* race_dat = Race_data[i];
// Pick a color - try the official race color first
    if (color_free[ race_dat->color ]) {
      kingdom->color = race_dat->color;
      color_free[ race_dat->color ] = false;

    } else {
      std::vector<nc_color> colors = race_dat->kingdom_colors;
// Remove any already-used colors
      for (int i = 0; i < colors.size(); i++) {
        if (!color_free[ colors[i] ]) {
          colors.erase(colors.begin() + i);
          i--;
        }
      }
      if (colors.empty()) { // Can't use official colors; use a random one
        std::vector<nc_color> free_colors;
        for (int i = 1; i < c_null; i++) {  // Start at 1 to skip c_black
          if (color_free[i]) {
            free_colors.push_back( nc_color(i) );
          }
        }
        if (free_colors.empty()) {  // Shouldn't ever happen - 16 kingdoms!
          kingdom->color = nc_color( rng(1, c_null - 1) );
        } else {
          int index = rng(0, free_colors.size() - 1);
          kingdom->color = free_colors[index];
          color_free[ free_colors[index] ] = false;
        }
      } else {  // We can use an official color!
        int index = rng(0, colors.size() - 1);
        kingdom->color = colors[index];
        color_free[ colors[index] ] = false;
      }
    } // if (!color_free[race_dat->color])

// Place the kingdom.
    if (kingdom->place_in_world(world)) {
      Kingdoms.push_back( kingdom );  // ...and add to our list.
    }
  } // for (int i = 1; i < RACE_MAX; i++)
}

Kingdom::Kingdom()
{
  uid = -1;
  race = RACE_NULL;
  color = c_ltgray;
}

Kingdom::~Kingdom()
{
}

// size defaults to STANDARD_KINGDOM_SIZE (see kingdom.h)
bool Kingdom::place_in_world(World_map* world, int size)
{
  if (!world) {
    debugmsg("Kingdom::place_in_world() called with NULL world!");
    return false;
  }

  Race_datum* race_dat = Race_data[race];
// Get several locations to try
  std::vector<Point> locations_to_try;
  for (int i = 0; i < 20; i++) {
    Point p( rng(size * 2, WORLD_MAP_SIZE - (size * 2 + 1)),
             rng(size * 2, WORLD_MAP_SIZE - (size * 2 + 1)) );
    locations_to_try.push_back(p);
  }

// Figure out which is the best
  int best_value = -1000;  // We'll tolerate slightly-negative values.
  int best_index = -1;
  for (int i = 0; i < 20; i++) {
    int value = 0;
    Point p = locations_to_try[i];
    for (int x = p.x - size; value != -999999 && x <= p.x + size; x++) {
      for (int y = p.y - size; value != -999999 && y <= p.y + size; y++) {
        if (world->get_kingdom_id(x, y) != -1) {
          value = -999999; // There's already a kingdom here, it's no good.
        } else {
          Map_type type = world->get_map_type(x, y);
// Multiply the value based on distance (closer = more important)
          int dist = rl_dist(p, Point(x, y));
          int multiplier = 100;
// All terrain within 25% of our radius is valued equally.
          if (dist > size / 4) {
// Adjust so that dist = size / 4 means 0 lost, dist = size means 100 lost
            dist -= size / 4;
// 4/3 times the distance (which ranges from 0%-75%) times 100, divided by size.
            multiplier -= (400 * dist) / (size * 3);
          }
          if (race_dat->map_type_values.count(type)) {
            value += multiplier * race_dat->map_type_values[type];
          }
        }
      } // for (all y in range)
    } // for (all x in range)
// Check if this is the new best location.
    if (value > best_value) {
      best_value = value;
      best_index = i;
    }
  } // for (int i = 0; i < 20; i++)

  if (best_index == -1) { // None of our locations were any good!
    return false;
  }

// Mark the world map as claimed!
  Point loc = locations_to_try[best_index];
  for (int x = loc.x - size; x <= loc.x + size; x++) {
    for (int y = loc.y - size; y <= loc.y + size; y++) {
      world->set_kingdom_id(x, y, uid);
    }
  }

  return true;
}
