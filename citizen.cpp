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

std::string citizen_type_name(Citizen_type type)
{
  switch (type) {
    case CIT_NULL:      return "NULL";
    case CIT_SLAVE:     return "slave";
    case CIT_PEASANT:   return "peasant";
    case CIT_MERCHANT:  return "merchant";
    case CIT_BURGHER:   return "burgher";
    case CIT_NOBLE:     return "noble";
    case CIT_MAX:       return "BUG - CIT_MAX";
    default:            return "Unnamed Citizen_type";
  }
  return "BUG - Escaped citizen_type_name() switch";
}
