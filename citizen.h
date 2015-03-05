#ifndef _CITIZEN_H_
#define _CITIZEN_H_

#include "resource.h"
#include <string>
#include <vector>
#include <map>
#include <istream>

enum Citizen_type
{
  CIT_NULL = 0,
  CIT_SLAVE,
  CIT_PEASANT,
  CIT_MERCHANT,
  CIT_BURGHER,
  CIT_MAX
};

Citizen_type lookup_citizen_type(std::string name);
std::string citizen_type_name(Citizen_type type, bool plural = false);

struct Citizen_amount
{
  Citizen_type type;
  int amount;

  Citizen_amount() { type = CIT_NULL; amount = 0; }

  Citizen_amount(Citizen_type T, int A = 0) :
    type (T), amount (A) {}
  
};

// Morale gets modified by a variety of causes.
enum Morale_mod_type
{
  MORALE_MOD_NULL = 0,

// Good modifiers
  MORALE_MOD_LUXURY,    // Always disappears after 1 turn!
  MORALE_MOD_BUILDING,  // Always disappears after 1 turn!
  MORALE_MOD_FESTIVAL,

// Bad modifiers
  MORALE_MOD_HUNGER,
  MORALE_MOD_DEATH,

  MORALE_MOD_MAX
};

Morale_mod_type lookup_morale_mod_type(std::string name);
std::string morale_mod_type_name(Morale_mod_type type);

struct Morale_modifier
{
  Morale_modifier( Morale_mod_type T = MORALE_MOD_NULL, int A = 0,
                   Resource L = RES_NULL, std::string S = "" ) :
    type (T), amount (A), luxury (L), term (S) { }

// get_name() returns morale_mod_type_name(type) unless it's MORALE_MOD_LUXURY,
// in which case we refer to the resource name.
  std::string get_name(); // returns morale_mod_type_name(type)

  std::string save_data();
  bool load_data(std::istream& data);

  Morale_mod_type type;
  int amount; // Mesaured in 1/10th of a morale percentage.
  Resource luxury;  // For MORALE_MOD_LUXURY, we need the resource consumed
  std::string term; // Other mods want some specific term to describe them
};

class City;

struct Citizens
{
  Citizens();
  ~Citizens();

  std::string save_data();
  bool load_data(std::istream& data);

// ACCESSORS

  int get_unemployed();
  int get_income();
  int get_morale_percentage();
  int get_starvation_chance();

  std::vector<std::string> get_morale_mods(bool colorize = false);
  std::vector<int>         get_morale_mod_amounts();


// MUTATORS

  void reset(); // Set all values to 0 (don't change type though!)

/* pick_luxuries() attempts to set luxury_demands.  It looks at the resources
 * owned by <city>.  For each Luxury_type, if luxury_demands is not set and
 * city owns one or more resources that belongs to that type, we randomly pick
 * one (weighted by the amount <city> owns).  If luxury_demands IS set, then we
 * do the same random pick, but strongly weight things towards NOT changing.
 */
  void pick_luxuries(City* city);
// consume_luxuries() looks at consumption[] and attempts to deduct those
// resources from city, gaining a Morale_modifier in return.  If city doesn't
// have the resources, it's removed from consumption[].
  void consume_luxuries(City* city);

  void decrease_morale_mods();
// add_possession() returns the number of items we did not take.  We will only
// take items up to the limit (set by the Resource_datum)
  int add_possession(Resource_amount res);
  int add_possession(Resource res, int amount);

// We can't just use default parameters for add_morale_modifier(), since we have
// one version with a Resource but no string, and another with a string but no
// resource (plus one with neither, and the full version has both).
  void add_morale_modifier(Morale_mod_type type, int amount);
  void add_morale_modifier(Morale_mod_type type, int amount,
                           Resource luxury);
  void add_morale_modifier(Morale_mod_type type, int amount,
                           std::string term);
  void add_morale_modifier(Morale_mod_type type, int amount, Resource luxury,
                           std::string term);

  void add_citizens   (int amount);
  void remove_citizens(int amount);


// DATA

  Citizen_type type;

  int count;
  int employed;
  int wealth;
  int tax_morale;
  int morale_points;
  int starvation;

// consumption is how much of each resource we consume daily.  Generally these
// will be luxuries.  This does NOT include food.
  std::map<Resource,int> consumption;
  std::map<Luxury_type,Resource> luxury_demands;  // Which luxury of each type?
  std::vector<Morale_modifier> morale_modifiers;

};

int citizen_food_consumption(Citizen_type type);
int citizen_idle_income(Citizen_type type); // Per 100 citizens.

enum Cause_of_death
{
  DEATH_NULL = 0,

  DEATH_UNKNOWN,  // Try to avoid using this!

  DEATH_STARVATION,
  DEATH_HUNTING,

  DEATH_MAX
};

// Format is "Citizens ______" e.g. "starved to death" or "were killed hunting"
std::string cause_of_death_text(Cause_of_death cause, bool plural);

#endif
