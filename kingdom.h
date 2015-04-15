#ifndef _KINGDOM_H_
#define _KINGDOM_H_

#include "city.h"
#include "color.h"
#include "race.h"
#include "world.h"
#include <vector>
#include <string>
#include <istream>

// This is the radius around a city which we lay claim to.
#define KINGDOM_CLAIM_RADIUS 8
// This is how many "points" we have to expand; adding a city generally costs 10
#define KINGDOM_EXPANSION_POINTS 200

class Kingdom
{
public:
  Kingdom();
  ~Kingdom();

  void set_game(Game* g);

  std::string save_data();
  bool load_data(std::istream& data);

// Building the kingdom
// called once for each kingdom; it places its capital, the epicenter of the
// territory.
  bool place_capital     (World_map* world, int radius = KINGDOM_CLAIM_RADIUS);
// Places a duchy city and claims territory around it.  Decreases
// expansion_points.
  bool place_duchy_seat  (World_map* world, int& expansion_points);
// Places several minor cities around a duchy seat.  They appear within <radius>
// tiles of the duchy seat.
  void place_minor_cities(World_map* world, int radius = KINGDOM_CLAIM_RADIUS);
  void build_road(World_map* world, City* start, City* end);
  void expand_boundaries(World_map* world);
  void setup_trade_routes(int base_percent);  // param is for progress bar

// Data
  int uid;
  Race race;
  nc_color color;

  City* capital;
  std::vector<City*> dukes;
  std::vector<City*> cities;

// Kingdom boundaries
  int most_west, most_north, most_east, most_south;

private:
  Point pick_best_point(World_map* world, std::vector<Point> points_to_try,
                        int radius = KINGDOM_CLAIM_RADIUS);
  void add_city(World_map* world, Point loc, City_type type,
                int radius = KINGDOM_CLAIM_RADIUS);
  void claim_territory(World_map* world, Point p);

// Data
  Game* game;

  std::vector<Point> city_locations;
};

// See kingdom.cpp
void init_kingdoms(Game* game, World_map* world);

#endif
