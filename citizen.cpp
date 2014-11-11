#include "citizen.h"
#include "stringfunc.h"
#include "rng.h"

Citizens::Citizens()
{
  count         = 0;
  employed      = 0;
  wealth        = 0;
  tax_morale    = 0;
  morale_points = 0;
  starvation    = 0;
}

Citizens::~Citizens()
{
}

int Citizens::get_unemployed()
{
  if (employed >= count) {
    return 0;
  }
  return count - employed;
}

int Citizens::get_income()
{
// Obviously, don't include income from government jobs (i.e. areas/buildings)
  int ret = 0;

// Idle citizens do produce income though!
  int idle = get_unemployed();
  ret += (idle * citizen_idle_income(type)) / 100;

// TODO: Other sources of income?

  return ret;
}

int Citizens::get_morale_percentage()
{
  if (count == 0) {
    return 0;
  }
  int ret = tax_morale;
  for (int i = 0; i < morale_modifiers.size(); i++) {
    ret += morale_modifiers[i].amount / 10;
  }
  for (int i = 0; i < RES_MAX; i++) {
    Resource res = Resource(i);
    if (possessions.count(res)) {
// TODO: Make some resources more effective?  Right now, all give 10%
      ret += (10 * possessions[res]) / count;
    }
  }
  return ret;
}

int Citizens::get_starvation_chance()
{
  if (count == 0) {
    return 0;
  }
  int ret = starvation / count;
  int remainder = starvation % count;
  if (rng(1, count) <= remainder) {
    ret++;
  }
  return ret;
}

void Citizens::decrease_morale_mods()
{
  for (int i = 0; i < morale_modifiers.size(); i++) {
    morale_modifiers[i].amount -= 1;
    if (morale_modifiers[i].amount <= 0) {
      morale_modifiers.erase( morale_modifiers.begin() + i);
      i--;
    }
  }
}

int Citizens::add_possession(Resource_amount res)
{
  return add_possession(res.type, res.amount);
}

int Citizens::add_possession(Resource res, int amount)
{
  int had = 0;
  if (possessions.count(res)) {
    had = possessions[res];
  }
  if (had + amount > count) {
    int left = amount - (count - had);
    possessions[res] = amount;
    return left;
  }
  if (possessions.count(res)) {
    possessions[res] += amount;
  } else {
    possessions[res] = amount;
  }
  return 0;
}

void Citizens::add_morale_modifier(Morale_mod_type type, int amount)
{
// Check if we have a modifier of that type.
  for (int i = 0; i < morale_modifiers.size(); i++) {
    if (morale_modifiers[i].type == type) {
      morale_modifiers.amount += amount;
      return;
    }
  }
  Morale_modifier mod(type, amount);
  morale_modifiers.push_back(mod);
}

void Citizens::add_citizens(int amount)
{
  if (amount <= 0) {
    return;
  }
  count += amount;
  morale_points += 50 * amount; // Each one comes into the world with 50% morale
}

void Citizens::remove_citizens(int amount)
{
  if (amount <= 0) {
    return;
  }
// get_morale_percentage() basically returns the morale points per citizen.
// When we remove citizens, we need to remove their morale points too.
  morale_points -= amount * get_morale_percentage();
// Ditto starvation.
  starvation    -= amount * get_starvation_chance();
  count -= amount;
  if (count < 0) {
    morale_points = 0;
    count = 0;
  }
}

Citizen_type lookup_citizen_type(std::string name)
{
  name = no_caps( trim( name ) );
  for (int i = 0; i < CIT_MAX; i++) {
    Citizen_type ret = Citizen_type(i);
    if (name == citizen_type_name(ret)) {
      return ret;
    }
  }
  return CIT_NULL;
}

// plural defaults to false
std::string citizen_type_name(Citizen_type type, bool plural)
{
  switch (type) {
    case CIT_NULL:      return "NULL";
    case CIT_SLAVE:     return (plural ? "slaves" : "slave");
    case CIT_PEASANT:   return (plural ? "peasants" : "peasant");
    case CIT_MERCHANT:  return (plural ? "merchants" : "merchant");
    case CIT_BURGHER:   return (plural ? "burghers" : "burgher");
    case CIT_MAX:       return "BUG - CIT_MAX";
    default:            return "Unnamed Citizen_type";
  }
  return "BUG - Escaped citizen_type_name() switch";
}

Morale_mod_type lookup_morale_mod_type(std::string name)
{
  name = no_caps( trim( name ) );
  for (int i = 0; i < MORALE_MOD_MAX; i++) {
    Morale_mod_type ret = Morale_mod_type(i);
    if (name == no_caps( morale_mod_type_name(ret) ) ) {
      return ret;
    }
  }
  return MORALE_MOD_NULL;
}

std::string morale_mod_type_name(Morale_mod_type type)
{
  switch (type) {
    case MORALE_MOD_NULL:     return "NULL";
    case MORALE_MOD_FESTIVAL: return "festival";
    case MORALE_MOD_HUNGER:   return "hunger";
    case MORALE_MOD_DEATH:    return "death";
    case MORALE_MOD_MAX:      return "BUG - MORALE_MOD_MAX";
    default:                  return "BUG - Unnamed Morale_mod_type";
  }
  return "BUG - Escaped morale_mod_type_name() switch!";
}

std::string Morale_modifier::get_name()
{
  return morale_mod_type_name(type);
}

// This is per 100 citizens!
int citizen_food_consumption(Citizen_type type)
{
  switch (type) {
    case CIT_NULL:      return   0;
    case CIT_SLAVE:     return 500;
    case CIT_PEASANT:   return 100;
    case CIT_MERCHANT:  return 140;
    case CIT_BURGHER:   return 180;
    case CIT_MAX:       return   0;
    default:            return   0;
  }
  return 0;
}

int citizen_idle_income(Citizen_type type)
{
  switch (type) {
    case CIT_NULL:      return   0;
    case CIT_SLAVE:     return   0;
    case CIT_PEASANT:   return  15;
    case CIT_MERCHANT:  return 100;
    case CIT_BURGHER:   return 300;
    case CIT_MAX:       return   0;
    default:            return   0;
  }
  return 0;
}

std::string cause_of_death_text(Cause_of_death cause, bool plural)
{
  switch (cause) {
    case DEATH_NULL:        return "NULL";
    case DEATH_UNKNOWN:     return "died mysteriously";
    case DEATH_STARVATION:  return "starved to death";

    case DEATH_HUNTING:
      if (plural) {
        return "were killed while hunting";
      }
      return "was killed while hunting";

    case DEATH_MAX:         return "BUG - DEATH_MAX";
    default:                return "Unnamed Cause_of_death";
  }
  return "BUG - Escaped cause_of_death_name() switch!";
}
