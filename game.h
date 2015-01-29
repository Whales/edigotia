#ifndef _GAME_H_
#define _GAME_H_

#include "date.h"
#include "race.h"
#include <string>
#include <vector>

class City;
class Player_city;
class World_map;
class Kingdom;

class Game
{
public:
  Game();
  ~Game();

// Initialization
// returns false if we cancel when picking a city site
  bool start_new_game();
// returns true if we've generated or loaded our world
  bool is_world_ready();
// Also loads kingdoms.  Returns false if no saved world exists
  bool load_world();
// returns false on failure
  bool generate_world();

// Saves the game data and the current city in SAVE_DIR/cities/<cityname>.sav
  bool save_game();
// Loads a game from the specified file
  bool load_game(std::string filename);

// Mutators
  void advance_time(int days = 1, City* city = NULL);

// Accessors
/* If we pass a length to get_date_str() (i.e. length != -1), then we colorize
 * it properly (white on blue) and make sure it fills the field (put in blue-on-
 * blue x until the tagless length matches the parameter).
 */
  Date get_date();
  std::string get_date_str(int length = -1);

  Kingdom* get_kingdom_for_race(Race race);

  int get_city_uid(); // Returns next_city_uid and advances next_city_uid.

  World_map* world;
  std::vector<Kingdom*> kingdoms;
  Player_city* city;

private:
  void generate_kingdoms();
  bool save_kingdoms();
  bool load_kingdoms();

  Date date;
  int next_city_uid;
  bool world_ready;
};

#endif
