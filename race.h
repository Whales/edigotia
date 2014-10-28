#ifndef _RACE_H_
#define _RACE_H_

#include "citizen.h"  // Some race values are Citizen_type-dependent.
#include "color.h"
#include "map.h"      // For Map_type
#include <string>

enum Race
{
  RACE_NULL = 0,

// Standard fantasy races
  RACE_HUMAN,
  RACE_ELF,
  RACE_DWARF,
  RACE_ORC,

/*
// Expanded-standard fantasy races
  RACE_GOBLIN,
  RACE_KOBOLD,
  RACE_OGRE,
  RACE_TROLL,
  RACE_GNOME,
  RACE_HALFLING,

// Rare/obscure/highly specialized fantasy races
  RACE_NAGA,
  RACE_CENTAUR,
  RACE_MINOTAUR,
  RACE_LIZARDMAN,
  RACE_GNOLL,

// Advanced/strange fantasy races
  RACE_FORMICANT, // Ant people
  RACE_ARACHNID,  // Spider people
  RACE_GOLEM,
  RACE_GARGOYLE,
  RACE_RAKSHASA,
  RACE_DRACONIAN,
  RACE_DENDROID,  // Tree people
  RACE_FELID,     // Cat people
  RACE_SPRIGGAN,  // Fairy-like creature
*/

  RACE_MAX
};

enum Race_skill
{
  SKILL_NULL = 0,

// Area-related skills
  SKILL_FARMING,
  SKILL_MINING,
  SKILL_FORESTRY, // Includes logging

  SKILL_MAX
};

Race_skill lookup_race_skill(std::string name);
std::string race_skill_name(Race_skill skill);

struct Race_datum
{
  Race_datum();
  ~Race_datum();

  int uid;  // Probably don't need it, but why not
  std::string name;
  std::string plural_name;
  std::string adjective;  // "Buying ____ weapons.", e.g. "orcish"

  nc_color color;


// PLACEMENT & CIVILIZATION

  Map_type preferred_map_type;


// GENERAL RACE STATS

  int food_consumption; // Default is 100


// CITIZEN-TYPE-BASED STATS

  int starting_population[CIT_MAX];
  int birth_rate[CIT_MAX];  // Lower is faster; this many citizens for 1 point
/* The citizen_ratio of each Citizen_type represents the number of the type
 * below we need to support 1 of this type.  For instance, if
 * citizen_ratio[CIT_MERCHANT] = 15, we need 15 peasants to support 1
 * merchant (30 to support 2, 150 to support 10, etc).
 * citizen_ratio and morale_requirement are obviously only needed for
 * CIT_MERCHANT and CIT_BURGHER.
 */
  int citizen_ratio[CIT_MAX];
  int morale_requirement[CIT_MAX];  // Morale required to birth this CitizenType
  int low_tax_rate[CIT_MAX];  // Tax rates at/below this give a morale boost.
  int high_tax_rate[CIT_MAX]; // Tax rates at/above this give a big morale minus



// SKILLS ETC

  int skill_level[SKILL_MAX];

};

// Defined in race_data.cpp
extern Race_datum* Race_data[RACE_MAX];
void init_races();


#endif
