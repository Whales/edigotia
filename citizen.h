#ifndef _CITIZEN_H_
#define _CITIZEN_H_

#include <string>

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
int citizen_food_consumption(Citizen_type type);
int citizen_idle_income(Citizen_type type); // Per 100 citizens.

struct Citizen_amount
{
  Citizen_type type;
  int amount;

  Citizen_amount() { type = CIT_NULL; amount = 0; }

  Citizen_amount(Citizen_type T, int A = 0) :
    type (T), amount (A) {}
  
};

#endif
