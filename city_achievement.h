#ifndef _CITY_ACHIEVEMENT_H_
#define _CITY_ACHIEVEMENT_H_

#include <string>

// City achievements are waypoints for cities, certain conditions that they've
// met.  They're used, for example, for unlocking areas and buildings.

enum City_achievement_type
{
  CITY_ACHIEVE_NULL = 0,

  CITY_ACHIEVE_ORES,      // Mine any ores
  CITY_ACHIEVE_POP,       // Have a certain population

  CITY_ACHIEVE_MAX
};

City_achievement_type lookup_city_achievement_type(std::string name);
std::string city_achievement_type_name(City_achievement_type type);

struct City_achievement
{
  City_achievement(City_achievement_type T = CITY_ACHIEVE_NULL,
                   int VA = 0, int VB = 0) :
    type (T), value_a (VA), value_b (VB)
    { }

  std::string get_description();

  City_achievement_type type;

/* values has different meanings depending on the achievement type.  For 
 * instance, for CITY_ACHIEVE_POP, we expect two values; the first is the
 * Citizen_type and the second is the population amount.
 */
  int value_a, value_b;
};

#endif
