#include "game.h"
#include "stringfunc.h"

Game::Game()
{
}

Game::~Game()
{
}

void Game::init()
{
  date = Date(1400, 5, 1);
}

void Game::advance_time(int days)
{
// days defaults to 1.
  date.advance(days);
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
