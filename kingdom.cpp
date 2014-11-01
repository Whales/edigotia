#include "kingdom.h"
#include "rng.h"
#include "window.h" // For debugmsg()
#include "geometry.h" // For rl_dist()
#include "ai_city.h"
#include <sstream>

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
    popup_nowait("Initializing Kingdoms (%d of %d)...", i, RACE_MAX - 1);

    Kingdom* kingdom = new Kingdom;
    kingdom->uid = i - 1;
    kingdom->race = Race(i);
    Race_datum* race_dat = Race_data[i];
// Pick a color - try the official race color first
    if (race_dat->color < c_dkgray && color_free[ race_dat->color ]) {
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
// Start at 1 to skip c_black; stop at c_dkgray to skip bright colors
        for (int i = 1; i < c_dkgray; i++) {
          if (color_free[i]) {
            free_colors.push_back( nc_color(i) );
          }
        }
        if (free_colors.empty()) {  // 8 kingdoms used already!
          kingdom->color = nc_color( rng(1, c_dkgray - 1) );
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
    if (kingdom->place_capital(world)) {
      Kingdoms.push_back( kingdom );  // ...and add to our list.
    }
  } // for (int i = 1; i < RACE_MAX; i++)

/* Now, expand the kingdoms by placing duchy seats.  To keep things fair, each
 * kingdom gets to place a single city/duchy at a time.  We go through our list,
 * each kingdom taking a turn, until all kingdoms are out of points.
 */
  std::vector<int> points, kingdom_index;
  for (int i = 0; i < Kingdoms.size(); i++) {
    points.push_back( KINGDOM_EXPANSION_POINTS ); // Defined in kingdom.h
    kingdom_index.push_back( i );
  }

  int iteration = 0, max_points = points.size() * KINGDOM_EXPANSION_POINTS;
  while (!kingdom_index.empty()) {
    iteration++;
    int total_points = 0;
    for (int i = 0; i < points.size(); i++) {
      total_points += points[i];
    }
    int percent = (100 * (max_points - total_points)) / max_points;
    popup_nowait("Placing duchies... [%d%%%%%%%%]", percent);
    for (int i = 0; i < kingdom_index.size(); i++) {
      Kingdom* kingdom = Kingdoms[ kingdom_index[i] ];
      if (!kingdom->place_new_city(world, points[i])) {
        kingdom_index.erase( kingdom_index.begin() + i );
        points.erase( points.begin() + i );
      }
    }
  }

// Now each kingdom gets to place minor cities in its duchies.
  for (int i = 0; i < Kingdoms.size(); i++) {
    int percent = (100 * (i + 1)) / Kingdoms.size();
    popup_nowait("Placing minor cities... [%d%%%%%%%%]", percent);
    Kingdoms[i]->place_minor_cities(world);
  }

// Finally, swell the territory claimed by each kingdom.
  int expansions = 3;
  for (int n = 0; n < expansions; n++) {
    for (int i = 0; i < Kingdoms.size(); i++) {
      popup_nowait("Expanding territories... (%d/%d; %d/%d)",
                   n + 1, expansions, i + 1, Kingdoms.size());
      Kingdoms[i]->expand_boundaries(world);
    }
  }
}

Kingdom::Kingdom()
{
  uid = -1;
  race = RACE_NULL;
  color = c_ltgray;
  capital = NULL;
  most_west  = WORLD_MAP_SIZE - 1;
  most_north = WORLD_MAP_SIZE - 1;
  most_east  = 0;
  most_south = 0;
}

Kingdom::~Kingdom()
{
  if (capital) {
    delete capital;
  }
  for (int i = 0; i < dukes.size(); i++) {
    delete (dukes[i]);
  }
  for (int i = 0; i < cities.size(); i++) {
    delete (cities[i]);
  }
}

// size defaults to KINGDOM_CLAIM_RADIUS (see kingdom.h)
bool Kingdom::place_capital(World_map* world, int radius)
{
  if (!world) {
    debugmsg("Kingdom::place_in_world() called with NULL world!");
    return false;
  }

// Get several locations to try
  std::vector<Point> locations_to_try;
  for (int i = 0; i < 40; i++) {
    Point p( rng(radius * 2, WORLD_MAP_SIZE - (radius * 2 + 1)),
             rng(radius * 2, WORLD_MAP_SIZE - (radius * 2 + 1)) );
    locations_to_try.push_back(p);
  }

// Figure out which is the best
  Point loc = pick_best_point(world, locations_to_try, radius);

// Add our capital there.
  add_city(world, loc, CITY_TYPE_CAPITAL);
  return true;
}

bool Kingdom::place_new_city(World_map* world, int& expansion_points)
{
  if (!world) {
    debugmsg("Kingdom::place_new_city() called with NULL world!");
    return false;
  }
  if (expansion_points <= 0) {
    return false;
  }

// Check out all points near existing cities.
  std::vector<Point> points_to_try;
  for (int i = 0; i < city_locations.size(); i++) {
    Point p = city_locations[i];
    int max_dist = KINGDOM_CLAIM_RADIUS * 1.5;
// Don't pick any points that'll cost more than we have.
    if (max_dist > expansion_points) {
      max_dist = expansion_points;
    }
    for (int dist = KINGDOM_CLAIM_RADIUS - 2; dist < max_dist; dist++) {
// points_at_exact_distance() is in geometry.{h,cpp}
      std::vector<Point> circle = points_at_exact_distance(p, dist);
      for (int n = 0; n < circle.size(); n++) {
        if (circle[n].x >= 0 && circle[n].x < WORLD_MAP_SIZE &&
            circle[n].y >= 0 && circle[n].y < WORLD_MAP_SIZE &&
            world->get_map_type(circle[n].x, circle[n].y) != MAP_OCEAN) {
          points_to_try.push_back( circle[n] );
        }
      }
    }
  }

  Point loc = pick_best_point(world, points_to_try, KINGDOM_CLAIM_RADIUS);

  if (loc.x < 0) {
    return false; // Couldn't pick a point!
  }
// Figure out the cost (shortest distance to an existing city).
  int lowest_cost = 999;
  for (int i = 0; i < city_locations.size(); i++) {
    int cost = rl_dist( city_locations[i], loc );
    if (cost < lowest_cost) {
      lowest_cost = cost;
    }
  }

// Pay the cost.
  expansion_points -= lowest_cost;

// Is this a duchy seat?
  City_type type = CITY_TYPE_DUCHY;

// Place the city.
  add_city(world, loc, type, KINGDOM_CLAIM_RADIUS);
  return true;
}

void Kingdom::place_minor_cities(World_map* world, int radius)
{
  //std::stringstream ss_debug;

  Race_datum* race_dat = Race_data[race];

  //ss_debug << race_dat->cluster_min << "<=>" << race_dat->cluster_max <<
              //std::endl;
// Copy city_locations since it'll change during this function.
  std::vector<Point> tmp_city_locations = city_locations;
// tmp_city-locations contains the locations of all duchy seats; for each one,
// place several "minor" (regular) cities.
  for (int i = 0; i < tmp_city_locations.size(); i++) {
    int percent = (100 * (i + 1)) / tmp_city_locations.size();
    popup_nowait("Placing minor cities [%d%%%%%%%%]", percent);
    Point parent_city = tmp_city_locations[i];
// Use our race to determine how many to place.
    std::vector<Point> new_city_locations;
    int num_cities = rng(race_dat->cluster_min, race_dat->cluster_max);

// Get a list of all possible locations, and their scores according to our race.
    std::vector<Point> possible_locations;
    std::vector<int>   scores;

    for (int x = parent_city.x - radius; x <= parent_city.x + radius; x++) {
      for (int y = parent_city.y - radius; y <= parent_city.y + radius; y++) {
// Only use this point if it's in our kingdom and not adjacent to the parent.
        int kingdom_id = world->get_kingdom_id(x, y);
        if (x >= 0 && x < WORLD_MAP_SIZE && y >= 0 && y < WORLD_MAP_SIZE &&
            (kingdom_id == uid || kingdom_id == -1) &&
            rl_dist(parent_city, Point(x, y)) > 1) {
// Figure out the score of the location, and insert it into our list in the
// proper position to sort the list by score.
// Randomize score a little bit so it's not always the same terrain.
          int score = rng(0, 10);
          if (one_in(10)) {
            score += rng(10, 50);
          }
          Map_type map_type = world->get_map_type(x, y);
          if (race_dat->map_type_value.count(map_type)) {
            score += race_dat->map_type_value[map_type];
          }
// Safety check
          if (possible_locations.size() != scores.size()) {
            debugmsg("locations %d, scores %d", possible_locations.size(),
                     scores.size());
          }
// Now find a place to insert this location.
          bool inserted = false;
          for (int n = 0; !inserted && n < possible_locations.size(); n++) {
            if (scores[n] < score ||
                (scores[n] == score && one_in(possible_locations.size() - n))) {
              inserted = true;
              possible_locations.insert( possible_locations.begin() + n,
                                         Point(x, y) );
              scores.insert( scores.begin() + n, score );
              if (possible_locations.size() > num_cities * 2) {
                possible_locations.pop_back();
                scores.pop_back();
              }
            }
          }
// If we never inserted, stick us at the back of the list.
          if (!inserted && possible_locations.size() < num_cities * 2) {
            possible_locations.push_back( Point(x, y) );
            scores.push_back( score );
          }
        } // if location is okay to use
      }
    }

// Okay, we've got all our possible locations; now select from the top N, where
// N is twice the number of cities we're placing.
    int list_size = num_cities * 2;
    if (list_size > possible_locations.size()) {
      list_size = possible_locations.size();
    }

// Now, for each city to place, pick an item from this final list.
    for (int n = 0;
         list_size > 0 && !possible_locations.empty() && n < num_cities;
         n++) {
      int index = rng(0, list_size - 1);
      Point city_loc = possible_locations[index];
      possible_locations.erase( possible_locations.begin() + index);
      list_size--;
// Ensure we're not adjacent to any new cities.
      bool okay = true;
      for (int j = 0; okay && j < new_city_locations.size(); j++) {
        int dist = rl_dist(city_loc, new_city_locations[j]);
        if (dist <= 1) {
          okay = false;
        }
      }
      if (okay) {
        new_city_locations.push_back( city_loc );
      } else {
        n--;
      }
    }
// Now place those new cities.
    ////ss_debug << "Duchy " << i << ": " << new_city_locations.size() <<
                //" cities" << std::endl;
/*
    if (new_city_locations.size() < race_dat->cluster_min) {
      debugmsg("Placing %d cities, min is %d.",
               new_city_locations.size(), race_dat->cluster_min);
    }
*/

    for (int n = 0; n < new_city_locations.size(); n++) {
      add_city(world, new_city_locations[n], CITY_TYPE_CITY, radius / 2);
    }

  } // for (int i = 0; i < tmp_city_locations.size(); i++)

  //debugmsg( ss_debug.str().c_str() );
}

Point Kingdom::pick_best_point(World_map* world,
                               std::vector<Point> points_to_try, int radius)
{
  if (!world) {
    debugmsg("Kingdom::pick_best_point() called with NULL World_map*!");
    return Point(-1, -1);
  }

  Race_datum* race_dat = Race_data[race];

// Points lost for being close to another kingdom
  int kingdom_penalty = radius * 5;

  int best_value = -1000;  // We'll tolerate slightly-negative values.
  int best_index = -1;
  for (int i = 0; i < points_to_try.size(); i++) {
    int value = 0;
    Point p = points_to_try[i];
// The center point MUST be unclaimed.
    if (world->get_kingdom_id(p) == -1) {

// Check out nearby terrain
      for (int x = p.x - radius; x <= p.x + radius; x++) {
        for (int y = p.y - radius; y <= p.y + radius; y++) {

// Multiply the value based on distance (closer = more important)
          int dist = rl_dist(p, Point(x, y));
          int multiplier = 100;
// All terrain within 25% of our radius is valued equally.
          if (dist > radius / 4) {
// Adjust so that dist = radius / 4 means 0 lost, dist = radius means 100 lost
            dist -= radius / 4;
// 4/3 times the distance (which ranges from 0%-75%) times 100, divided by size.
            multiplier -= (400 * dist) / (radius * 3);
          }

// Greatly increase the multiplier for the center tile - you know, the one our
// city will actually be on.
          if (x == p.x && y == p.y) {
            multiplier = 100 * (radius - 1) * (radius - 1);
          }

// Are we close to another kingdom?
          if (world->get_kingdom_id(x, y) != -1) {
            value -= multiplier * kingdom_penalty;
          } else {
            Map_type type = world->get_map_type(x, y);
            if (race_dat->map_type_value.count(type)) {
              value += multiplier * race_dat->map_type_value[type];
            }
          }
        } // for (all y in range)
      } // for (all x in range)
// Check if this is the new best location.
      if (value > best_value) {
        best_value = value;
        best_index = i;
      }
    } // if (world->get_kingdom_id(p) == -1)
  } // for (int i = 0; i < 20; i++)

  if (best_index == -1) { // None of our locations were any good!
    return Point(-1, -1);
  }

  return points_to_try[best_index];
}

void Kingdom::add_city(World_map* world, Point loc, City_type type, int radius)
{
  Race_datum* race_dat = Race_data[race];
// Mark the world map as claimed!
  for (int x = loc.x - radius; x <= loc.x + radius; x++) {
    for (int y = loc.y - radius; y <= loc.y + radius; y++) {
      Map_type type = world->get_map_type(x, y);
      int value = (50 / radius) * (radius - trig_dist( loc, Point(x, y) ));
      if (race_dat->map_type_value.count(type) &&
          race_dat->map_type_value[type] > 0) {
        value += race_dat->map_type_value[type];
      }
      if (world->get_map_type(x, y) != MAP_OCEAN && value >= 25) {
// Only claim non-ocean points with a decent value.
        claim_territory(world, Point(x, y));
      }
    }
  }

// Now actually add our city.
  AI_city* city = new AI_city;
  city->set_city_type(type);
  city->set_race(race);
  city->set_random_name();

  switch (type) {
    case CITY_TYPE_CAPITAL:
      if (capital) {
        delete capital;
      }
      capital = city;
      break;
    case CITY_TYPE_DUCHY:
      dukes.push_back(city);
      break;
    case CITY_TYPE_CITY:
    default:
      cities.push_back(city);
      break;
  }

// Finally, tell the world that this city is here.
  world->set_city(loc, city);

// And add it to our list of city locations.
  city_locations.push_back(loc);
}

void Kingdom::claim_territory(World_map* world, Point p)
{
  if (!world) {
    debugmsg("Kingdom::claim_territory() called with NULL world!");
    return;
  }

  if (world->get_kingdom_id(p) != -1) {
    return;
  }

  world->set_kingdom_id(p, uid);
// Set our border boundaries
  if (p.x < most_west) {
    most_west = p.x;
  }
  if (p.x > most_east) {
    most_east = p.x;
  }
  if (p.y < most_north) {
    most_north = p.y;
  }
  if (p.y > most_south) {
    most_south = p.y;
  }
}

void Kingdom::expand_boundaries(World_map* world)
{
  if (!world) {
    debugmsg("Kingdom::expand_boundaries() called with NULL world!");
    return;
  }

/* Store points that we'll need to claim, rather than immediately claiming them.
 * Immediately claiming would cause us to claim more tiles due to the ones we
 * just claimed.
 */
  std::vector<Point> points_to_claim;
  for (int x = most_west - 1; x <= most_east + 1; x++) {
    for (int y = most_north - 1; y <= most_south + 1; y++) {
// Check if we've claimed bordering tiles
      if (world->get_kingdom_id(x, y) == -1) {
        int bordering = 0, enemies = 0;;
        for (int bx = x - 1; bx <= x + 1; bx++) {
          for (int by = y - 1; by <= y + 1; by++) {
            if (world->get_kingdom_id(bx, by) == uid) {
              bordering++;
            } else if (world->get_kingdom_id(bx, by) != -1) {
              enemies++;
            }
          }
        }
        if (bordering >= enemies * 2 && bordering >= 3 &&
            world->get_map_type(x, y) != MAP_OCEAN) {
          points_to_claim.push_back( Point(x, y) );
        }
      }
    }
  }

  for (int i = 0; i < points_to_claim.size(); i++) {
    claim_territory(world, points_to_claim[i]);
  }
}
