#ifndef _RACE_H_
#define _RACE_H_

#include "citizen.h"  // Some race values are Citizen_type-dependent.
#include "color.h"
#include "map.h"      // For Map_type
#include <string>
#include <map>

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

// pos is "start" "middle" or "end" and indicates where to put it.
// The rest is a null-terminated list of C strings
  void add_city_names(std::string pos, ...);
  std::string get_city_name();

  int uid;  // Probably don't need it, but why not
  std::string name;
  std::string plural_name;
  std::string adjective;  // "Buying ____ weapons.", e.g. "orcish"

  nc_color color;
  std::vector<nc_color> kingdom_colors; // Acceptable colors for kingdoms

// PLACEMENT & CIVILIZATION

// Used for placement; how much we want to live near various map types.
  std::map<Map_type,int> map_type_value;
// To name a city of this race, we string together a start, middle, and end.
  std::vector<std::string> city_name_start, city_name_middle, city_name_end;

// GENERAL RACE VALUES

  int food_consumption; // Default is 100


// GAME START VALUES

  int starting_population[CIT_MAX];
  int starting_resources [RES_MAX];


// CITIZEN-TYPE-BASED VALUES

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
