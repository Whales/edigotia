#include "citizen.h"
#include "stringfunc.h"

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

Cause_of_death lookup_cause_of_death(std::string name)
{
  name = no_caps( trim( name ) );
  for (int i = 0; i < DEATH_MAX; i++) {
    Cause_of_death ret = Cause_of_death(i);
    if (name == no_caps( cause_of_death_name( ret ) ) ) {
      return ret;
    }
  }
  return DEATH_NULL;
}

std::string cause_of_death_name(Cause_of_death cause)
{
  switch (cause) {
    case DEATH_NULL:        return "NULL";
    case DEATH_UNKNOWN:     return "unknown causes";
    case DEATH_STARVATION:  return "starvation";
    case DEATH_HUNTING:     return "hunting";
    case DEATH_MAX:         return "BUG - DEATH_MAX";
    default:                return "Unnamed Cause_of_death";
  }
  return "BUG - Escaped cause_of_death_name() switch!";
}
