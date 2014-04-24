#ifndef _CITIZEN_H_
#define _CITIZEN_H_

enum Citizen_type
{
  CIT_NULL = 0,
  CIT_SLAVE,
  CIT_PEASANT,
  CIT_MERCHANT,
  CIT_BURGHER,
  CIT_NOBLE,
  CIT_MAX
};

struct Citizen_amount
{
  Citizen_type type;
  int amount;

  Citizen_amount() { type = CIT_NULL; amount = 0; }
};

#endif
