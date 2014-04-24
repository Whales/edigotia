#ifndef _RESOURCE_H_
#define _RESOURCE_H_

enum Resource
{
  RES_NULL = 0,

  RES_GOLD,
  RES_FOOD,
  RES_WOOD,
  RES_STONE,

  RES_MAX
};

struct Resource_cost
{
  Resource type;
  int amount;
};

#endif
