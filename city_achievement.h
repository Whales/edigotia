#ifndef _CITY_ACHIEVEMENT_H_
#define _CITY_ACHIEVEMENT_H_

// City achievements are waypoints for cities, certain conditions that they've
// met.  They're used, for example, for unlocking areas and buildings.

enum City_achievement_type
{
  CITY_ACHIEVE_NULL = 0,

  CITY_ACHIEVE_POP,       // Have a certain population

  CITY_ACHIEVE_MAX
};

struct City_achievement
{
  City_achievement(City_achievement_type T = CITY_ACHIEVE_NULL,
                   int VA = 0, int VB = 0) :
    type (T), value_a (VA), value_b (VB) { }

  City_achievement_type type;

/* value_a and value_b have different meanings depending on the achievement
 * type.  For instance, for CITY_ACHIEVE_POP, value_a is the Citizen_type and
 * value_b is the population amount.
 */
  int value_a;
  int value_b;
};

#endif
