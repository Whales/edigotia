#ifndef _RACE_H_
#define _RACE_H_

#include "citizen.h"    // Some race values are Citizen_type-dependent.
#include "color.h"      // Race_datum stores the race's color.
#include "map_type.h"   // For Map_type
#include "city_type.h"  // We track population size for each City_type.
#include <string>
#include <map>

enum Map_type;

enum Race
{
  RACE_NULL = 0,

// Standard fantasy races
  RACE_HUMAN,
  RACE_ELF,
  RACE_DWARF,
  RACE_ORC,

// Expanded-standard fantasy races
  RACE_GOBLIN,
  RACE_OGRE,
  RACE_TROLL,
  RACE_HALFLING,
  RACE_GNOME,

// Rare/obscure fantasy races
  RACE_MINOTAUR,
  RACE_RAKSHASA,
  RACE_NAGA,
/*
  RACE_LIZARDMAN,
  RACE_KOBOLD,

// Rare/obscure/highly specialized fantasy races
  RACE_CENTAUR,
  RACE_GNOLL,

// Advanced/strange fantasy races
  RACE_FORMICANT, // Ant people
  RACE_ARACHNID,  // Spider people
  RACE_GOLEM,
  RACE_GARGOYLE,
  RACE_DENDROID,  // Tree people
  RACE_DRACONIAN,
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
  SKILL_HUNTING,
  SKILL_LIVESTOCK,
  SKILL_MINING,
  SKILL_FORESTRY, // Includes logging

// Other in-city skills
  SKILL_CONSTRUCTION,

// Intercity skills
  SKILL_TRADE,
  SKILL_ESPIONAGE,

// Magic skills
  SKILL_MAGIC,
  SKILL_EARTH_MAGIC,
  SKILL_WATER_MAGIC,
  SKILL_AIR_MAGIC,
  SKILL_FIRE_MAGIC,
  SKILL_LIFE_MAGIC,
  SKILL_DEATH_MAGIC,

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

  std::string generate_help_text();

  int uid;  // Probably don't need it, but why not
  std::string name;
  std::string plural_name;
  std::string adjective;  // "Buying ____ weapons.", e.g. "orcish"
  std::string description;

  nc_color color;
  std::vector<nc_color> kingdom_colors; // Acceptable colors for kingdoms

// PLACEMENT & CIVILIZATION

// Used for placement; how much we want to live near various map types.
  std::map<Map_type,int> map_type_value;
// Used for travel; the travel cost of entering various terrains.
// Only use when we want to override the default (see map_type_data.cpp).
  std::map<Map_type,int> map_type_travel_cost;
// To name a city of this race, we string together a start, middle, and end.
  std::vector<std::string> city_name_start, city_name_middle, city_name_end;
// How many cities will be clustered around a duchy seat?
  int cluster_min, cluster_max;
// How big are our cities?  Min/max for each type (city, ducy, capital)
  int city_size_min[CITY_TYPE_MAX], city_size_max[CITY_TYPE_MAX];

// GENERAL RACE VALUES

  int base_combat;      // Applies to fighting & hunting
  int hp;               // hp for combat; default is 100
  int food_consumption; // Default is 100
  int warlikeness;      // Range is -3 to 3; default/norm is -2
  int life_value;       // Range is 1 to 5; default is 3


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

// DIPLOMACY STUFF

// relations is our default opinion of a city belonging to this race
  int relations[RACE_MAX];

// SKILLS ETC

  int skill_level[SKILL_MAX];

};

// Defined in race_data.cpp
extern Race_datum* Race_data[RACE_MAX];
void init_races();


#endif
