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
  Trade_route(Point _target, int _distance = -1);
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
};

#endif
