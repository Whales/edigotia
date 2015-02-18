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
  Morale_modifier( Morale_mod_type T = MORALE_MOD_NULL, int A = 0 ) :
    type (T), amount (A) { }

  std::string get_name(); // returns morale_mod_type_name(type)

  std::string save_data();
  bool load_data(std::istream& data);

  Morale_mod_type type;
  int amount; // Mesaured in 1/10th of a morale percentage.
};

struct Citizens
{
  Citizens();
  ~Citizens();

  std::string save_data();
  bool load_data(std::istream& data);

  Citizen_type type;

  int count;
  int employed;
  int wealth;
  int tax_morale;
  int morale_points;
  int starvation;

  std::map<Resource,int>  possessions;  // These modify morale.
  std::map<Luxury_type,Resource> luxury_demands;  // Which luxury of each type?
  std::vector<Morale_modifier> morale_modifiers;

  void reset(); // Set all values to 0 (don't change type though!)

  int get_unemployed();
  int get_income();
  int get_morale_percentage();
  int get_starvation_chance();

  std::vector<std::string> get_morale_mods(bool colorize = false);
  std::vector<int>         get_morale_mod_amounts();


  void decrease_morale_mods();
// add_possession() returns the number of items we did not take.  We will only
// take items up to the limit (set by the Resource_datum)
  int add_possession(Resource_amount res);
  int add_possession(Resource res, int amount);

  void add_morale_modifier(Morale_mod_type type, int amount);

  void add_citizens   (int amount);
  void remove_citizens(int amount);
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
