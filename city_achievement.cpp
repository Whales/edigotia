#include "city_achievement.h"
#include "citizen.h"    // For citizen_type_name()
#include "stringfunc.h" // For capitalize()
#include <sstream>

City_achievement_type lookup_city_achievement_type(std::string name)
{
  name = no_caps( trim( name ) );
  for (int i = 0; i < CITY_ACHIEVE_MAX; i++) {
    City_achievement_type ret = City_achievement_type(i);
    if (name == city_achievement_type_name(ret)) {
      return ret;
    }
  }
  return CITY_ACHIEVE_NULL;
}

std::string city_achievement_type_name(City_achievement_type type)
{
  switch (type) {
    case CITY_ACHIEVE_NULL:   return "NULL";
    case CITY_ACHIEVE_ORES:   return "ores";
    case CITY_ACHIEVE_POP:    return "population";
    case CITY_ACHIEVE_MAX:    return "BUG - CITY_ACHIEVE_MAX";
    default:                  return "BUG - Unnamed City_achievement_type";
  }
  return "BUG - Escaped City_achievement_type switch";
}

std::string City_achievement::get_description()
{
  std::stringstream ret;

  switch (type) {

    case CITY_ACHIEVE_NULL:
      return "None";

    case CITY_ACHIEVE_ORES:
      return "Have tin, copper, or iron ore";

    case CITY_ACHIEVE_POP:
      ret << "Have " << value_b << " " <<
             citizen_type_name( Citizen_type(value_a), (value_b > 1) );
      break;

    case CITY_ACHIEVE_MAX:
      return "BUG - CITY_ACHIEVE_MAX";

    default:
      return "BUG - Unknown City_achievement_type";

  }

  return capitalize( ret.str() );
}
