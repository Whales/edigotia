#include "city_type.h"
#include "stringfunc.h"

City_type lookup_city_type(std::string name)
{
  name = no_caps( trim( name ) );
  for (int i = 0; i < CITY_TYPE_MAX; i++) {
    City_type ret = City_type(i);
    if (name == no_caps( trim( city_type_name(ret) ) ) ) {
      return ret;
    }
  }
  return CITY_TYPE_NULL;
}

std::string city_type_name(City_type type)
{
  switch (type) {
    case CITY_TYPE_NULL:    return "NULL";
    case CITY_TYPE_CITY:    return "city";
    case CITY_TYPE_DUCHY:   return "duchy seat";
    case CITY_TYPE_CAPITAL: return "capital";
    case CITY_TYPE_MAX:     return "BUG - CITY_TYPE_MAX";
    default:                return "BUG - Unnamed City_type";
  }
  return "BUG - Escaped city_type_name() switch!";
}
