#include "game.h"
#include "city.h"
#include "player_city.h"
#include "stringfunc.h"
#include "world.h"
#include "kingdom.h"
#include "rng.h"
#include "globals.h"
#include "files.h"
#include <fstream>

Game::Game()
{
  next_city_uid = 0;
  world = new World_map;
  city = new Player_city;
  world_ready = false;
}

Game::~Game()
{
}

bool Game::start_new_game()
{
  date = Date(1400, 5, 1);

  if (!world_ready) {

    if (file_exists(SAVE_DIR + "world.sav")) {
      if (!query_yn("Load world from save file?")) {
        return false;
      }
      world->load_from_file(SAVE_DIR + "world.sav");

    } else {
      if (!query_yn("We need to generate a world first, is that okay?") ||
          !generate_world()) {
        return false;
      }
      world->save_to_file("world.sav");
      save_kingdoms();
    }

  }

// Pick our race first, so we know where to start placement.
  city->pick_race();

// Let the city pick a location in the world
// Start from the center of the appropriate kingdom.
  Point start;
  Kingdom* city_kingdom = get_kingdom_for_race(city->get_race());
  if (city_kingdom) {
    start.x = (city_kingdom->most_west  + city_kingdom->most_east ) / 2;
    start.y = (city_kingdom->most_north + city_kingdom->most_south) / 2;
  } else {
    debugmsg("Kingdom not found for %s.  %d kingdoms.",
             Race_data[city->get_race()]->name.c_str(), kingdoms.size());
    start = Point(WORLD_MAP_SIZE / 2, WORLD_MAP_SIZE / 2);
  }

  city->set_starting_tiles_seen();

  Point p = world->draw(start, &(city->world_seen));

  if (p.x == -1) {  // We canceled
    return false;
  }

// Put our city there.
  bool placed = false;
  while (!placed) {
    if (!city->world_seen.is_seen(p)) {
      popup("That's unexplored territory!");
    } else if (!world->get_city(p)) {
      city->generate_map(p);
      placed = city->place_keep();
    } else {
      popup("There is already a city there!");
    }
    if (!placed) {
      p = world->draw(p, &(city->world_seen)); // Try again
      if (p.x == -1) {  // We canceled
        return false;
      }
    }
  }

  city->location = p;
// Let us see a little more.
  city->mark_nearby_tiles_seen(4);
  world->set_city(p, city);
// Crude race picker; TODO: replace this.
  city->set_name();
  city->start_new_city();
  city->setup_trade_routes();
  city->set_starting_tiles_seen();

  return true;
}

bool Game::is_world_ready()
{
  return world_ready;
}

bool Game::load_world()
{
  if (!file_exists(SAVE_DIR + "world.sav")) {
    return false;
  }

  world->load_from_file(SAVE_DIR + "world.sav");
  world_ready = true;
  return load_kingdoms();
}

bool Game::generate_world()
{
  if (world) {
    delete world;
    world = new World_map;
  }
  world->generate();
  generate_kingdoms();
  world->save_to_file(SAVE_DIR + "world.sav");
  save_kingdoms();
  world_ready = true;
  return true;
}

bool Game::save_game()
{
// Save the player's city.
  std::ofstream fout;
  std::string filename = SAVE_DIR + "cities/" + city->get_name() + ".sav";
  fout << city->save_data();

  fout.close();

  save_kingdoms();
  world->save_to_file(SAVE_DIR + "world.txt");

  return true;
}

bool Game::load_game(std::string filename)
{
  std::ifstream fin;
  fin.open(filename.c_str());
  if (!fin.is_open()) {
    debugmsg("Couldn't open %s for reading.", filename.c_str());
    return false;
  }

  if (!city->load_data(fin)) {
    debugmsg("Game failed to load city.");
    return false;
  }

  load_world();

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

Kingdom* Game::get_kingdom_for_race(Race race)
{
  for (int i = 0; i < kingdoms.size(); i++) {
    if (kingdoms[i]->race == race) {
//debugmsg("Got kingdom %d/%d (uid %d) %s", i, kingdoms.size(), kingdoms[i]->uid, Point(kingdoms[i]->most_west, kingdoms[i]->most_north).str().c_str());
      return kingdoms[i];
    }
  }
  return NULL;
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

  if (!kingdoms.empty()) {
    for (int i = 0; i < kingdoms.size(); i++) {
      delete (kingdoms[i]);
    }
  }
  kingdoms.clear();

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
    popup_nowait("Placing minor cities... [%d%%%%%%%%] [%d/%d]",
                 percent, i + 1, kingdoms.size());
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

// And setup trade routes for all cities!
  for (int i = 0; i < kingdoms.size(); i++) {
    int base_percent = (100 * i) / kingdoms.size();
    kingdoms[i]->setup_trade_routes(base_percent);
  }
}

bool Game::save_kingdoms()
{
  std::ofstream fout;
  std::string filename = SAVE_DIR + "kingdoms.txt";
  fout.open(filename.c_str());
  if (!fout.is_open()) {
    debugmsg("Couldn't open %s/kingdoms.txt for saving.", SAVE_DIR.c_str());
    return false;
  }

  fout << date.save_data() << std::endl;
  fout << next_city_uid << std::endl;

  fout << kingdoms.size() << std::endl;
  for (int i = 0; i < kingdoms.size(); i++) {
    fout << kingdoms[i]->save_data() << std::endl;
  }

  fout.close();
  return true;
}

bool Game::load_kingdoms()
{
  if (!world_ready) {
    debugmsg("Loading kingdoms before world - not okay!");
    return false;
  }

  std::ifstream fin;
  std::string filename = SAVE_DIR + "kingdoms.txt";
  fin.open(filename.c_str());
  if (!fin.is_open()) {
    return false;
  }

  if (!date.load_data(fin)) {
    debugmsg("Game failed to load date.");
    return false;
  }

  fin >> next_city_uid;


  int num_kingdoms;
  fin >> num_kingdoms;
  for (int i = 0; i < num_kingdoms; i++) {
    Kingdom* tmp_kingdom = new Kingdom;
    if (!tmp_kingdom->load_data(fin)) {
      return false;
    }
    kingdoms.push_back(tmp_kingdom);
// Add the kingdom's cities to our world map
    world->set_city(tmp_kingdom->capital->location, tmp_kingdom->capital);
    for (int i = 0; i < tmp_kingdom->dukes.size(); i++) {
      City* duke = tmp_kingdom->dukes[i];
      world->set_city(duke->location, duke);
    }
    for (int i = 0; i < tmp_kingdom->cities.size(); i++) {
      City* city = tmp_kingdom->cities[i];
      world->set_city(city->location, city);
    }
  }

  return true;
}
