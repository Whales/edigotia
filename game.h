#ifndef _GAME_H_
#define _GAME_H_

#include "date.h"
#include <string>

class City;

class Game
{
public:
  Game();
  ~Game();

// Initialization
  void init();

// Mutators
  void advance_time(int days = 1, City* city = NULL);

// Accessors
/* If we pass a length to get_date_str() (i.e. length != -1), then we colorize
 * it properly (white on blue) and make sure it fills the field (put in blue-on-
 * blue x until the tagless length matches the parameter).
 */
  Date get_date();
  std::string get_date_str(int length = -1);

private:
  Date date;
};

#endif
