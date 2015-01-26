#include "trade.h"
#include <sstream>

Trade_route::Trade_route()
{
  distance = -1;
}

Trade_route::Trade_route(int _distance)
{
// TODO: Warn if _distance <= 0?
  distance = _distance;
}

Trade_route::Trade_route(const Trade_route& other)
{
  distance = other.distance;
}

Trade_route::~Trade_route()
{
}

std::string Trade_route::save_data()
{
  std::stringstream ret;
  ret << distance;

  return ret.str();
}

bool Trade_route::load_data(std::istream& data)
{
  data >> distance;

  return true;
}
