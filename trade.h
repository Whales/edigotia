#ifndef _TRADE_H_
#define _TRADE_H_

#include "geometry.h" // for Point
#include <string>
#include <istream>

class City;

// Trade_route ony has a destination city.  The origin city stores several
// Trade_routes.
struct Trade_route
{
  Trade_route();
  Trade_route(Point _target, int _distance = -1, int _overhead = -1);
  Trade_route(const Trade_route& other);
  ~Trade_route();

  std::string save_data();
  bool load_data(std::istream& data);

  Point target;
/* distance is not physical - it's the "trade distance," measured by the ease of
 * getting a caravan there.  Basically the same as the total road cost on an A*
 * route to our destination.
 */
  int distance;

/* overhead is a static price that must be paid (effectively lost - no one
 * receives this "payment") by the buyer when goods are bought via this trade
 * route.  It's mainly based off "distance" above, but is also affected by race,
 * and maybe other factors???
 */
  int overhead;
};

#endif
