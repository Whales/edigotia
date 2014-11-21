#ifndef _CITY_TYPE_H_
#define _CITY_TYPE_H_

#include <string>

enum City_type
{
  CITY_TYPE_NULL = 0,
  CITY_TYPE_CITY,     // Normal city; ruled by a duchy capital
  CITY_TYPE_DUCHY,    // Capital of a duchy; ruled by a capital
  CITY_TYPE_CAPITAL,  // Capital of a kingdom
  CITY_TYPE_MAX
};

City_type lookup_city_type(std::string name);
std::string city_type_name(City_type type);

#endif
