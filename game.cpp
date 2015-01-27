#include "game.h"
#include "city.h"
#include "city.h"
#include "player_city.h"
#include "stringfunc.h"
#include "world.h"
#include "kingdom.h"
#include "rng.h"

Game::Game()
{
  next_city_uid = 0;
  world = new World_map;
  city = new Player_city;
}

Game::~Game()
{
}

bool Game::start_new_game()
{
  date = Date(1400, 5, 1);

  world->generate();
  world->save_to_file("world.sav");
  generate_kingdoms();

// Let the city pick a location in the world
  Point p = world->draw();

  if (p.x == -1) {  // We hit ESC
    return false;
  }

// Put our city there.
  bool placed = false;
  while (!placed) {
    if (!world->get_city(p)) {
      city->set_world_map(world);
      city->generate_map(p);
      placed = city->place_keep();
    } else {
      popup("There is already a city there!");
    }
    if (!placed) {
      p = world->draw(p);  // We decided against that spot, pick a new one
    }
  }

  city->location = p;
  world->set_city(p, city);
// Crude race picker; TODO: replace this.
  city->pick_race();
  city->set_name();
  city->start_new_city(world);
  city->set_game(this);
  city->setup_trade_routes();

  return true;
}

void Game::advance_time(int days, City* city)
{
// days defaults to 1.
  date.advance(days);
// city defaults to NULL
  if (city) {
    for (int i = 0; i < days; i++) {
      city->do_turn();
    }
  }
}

Date Game::get_date()
{
  return date;
}

// TODO: Allow other formats of the date.
std::string Game::get_date_str(int length)
{
  if (length < 0) {
    return date.get_text();
  }
  std::string ret = "<c=white,blue>";
  ret += date.get_text();
  ret += "<c=blue,blue>";
  while (tagless_length(ret) < length) {
    ret += 'x';
  }
  ret += "<c=/>";
  return ret;
}

int Game::get_city_uid()
{
  int ret = next_city_uid;
  next_city_uid++;
  return ret;
}

void Game::generate_kingdoms()
{
  if (!world) {
    debugmsg("Game::generate_kingdoms() called with NULL world!");
    return;
  }

  bool color_free[c_null];  // c_null is the last color
  for (int i = 0; i < c_null; i++) {
    color_free[i] = true;
  }
// One kingdom for each race.  Start at 1 to skip RACE_NULL.
  for (int i = 1; i < RACE_MAX; i++) {
    popup_nowait("Initializing kingdoms (%d of %d)...", i, RACE_MAX - 1);

    Kingdom* kingdom = new Kingdom;
    kingdom->uid = i - 1;
    kingdom->set_game(this);
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
      kingdoms.push_back( kingdom );  // ...and add to our list.
    }
  } // for (int i = 1; i < RACE_MAX; i++)

/* Now, expand the kingdoms by placing duchy seats.  To keep things fair, each
 * kingdom gets to place a single city/duchy at a time.  We go through our list,
 * each kingdom taking a turn, until all kingdoms are out of points.
 */
  std::vector<int> points, kingdom_index;
  for (int i = 0; i < kingdoms.size(); i++) {
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
      Kingdom* kingdom = kingdoms[ kingdom_index[i] ];
      if (!kingdom->place_duchy_seat(world, points[i])) {
        kingdom_index.erase( kingdom_index.begin() + i );
        points.erase( points.begin() + i );
      }
    }
  }

// Now each kingdom gets to place minor cities in its duchies.
  for (int i = 0; i < kingdoms.size(); i++) {
    int percent = (100 * (i + 1)) / kingdoms.size();
    popup_nowait("Placing minor cities... [%d%%%%%%%%]", percent);
    kingdoms[i]->place_minor_cities(world);
  }

// Build roads from the capital to each duchy.  Building roads from the duchy to
// its minor cities is handled in place_minor_cities().
  for (int i = 0; i < kingdoms.size(); i++) {
    int percent = (100 * (i + 1)) / kingdoms.size();
    popup_nowait("Connecting duchies via road... [%d%%%%%%%%]", percent);
    for (int n = 0; n < kingdoms[i]->dukes.size(); n++) {
      City* capital = kingdoms[i]->capital;
      City* duke = kingdoms[i]->dukes[n];
      kingdoms[i]->build_road( world, capital, duke );
    }
  }

// Finally, swell the territory claimed by each kingdom.
  int expansions = 3;
  for (int n = 0; n < expansions; n++) {
    for (int i = 0; i < kingdoms.size(); i++) {
      popup_nowait("Expanding territories... (%d/%d; %d/%d)",
                   n + 1, expansions, i + 1, kingdoms.size());
      kingdoms[i]->expand_boundaries(world);
    }
  }
}
