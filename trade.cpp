#include "trade.h"
#include <sstream>

Trade_route::Trade_route()
{
  distance = -1;
}

// _distance and _overhead default to -1
Trade_route::Trade_route(Point _target, int _distance, int _overhead)
{
  target = _target;
// TODO: Warn if _distance or _overhead <= 0?
  distance = _distance;
  overhead = _overhead;
}

Trade_route::Trade_route(const Trade_route& other)
{
  target = other.target;
  distance = other.distance;
}

Trade_route::~Trade_route()
{
}

std::string Trade_route::save_data()
{
  std::stringstream ret;
  ret << target.x << " ";
  ret << target.y << " ";
  ret << distance << " ";

  return ret.str();
}

bool Trade_route::load_data(std::istream& data)
{
  data >> target.x >> target.y >> distance;

  return true;
}
