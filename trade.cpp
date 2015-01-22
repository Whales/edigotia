#include "trade.h"

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

