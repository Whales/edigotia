#include "resource.h"
#include "stringfunc.h"
#include "rng.h"

Mineral_amount Mineral_amount::make_small()
{
  Mineral_amount ret;
  ret.type = type;
  ret.amount = rng( amount * .1, amount * .2);
  return ret;
}

Mineral_amount Mineral_amount::randomize()
{
  Mineral_amount ret;
  ret.type = type;
  ret.amount = rng( amount * .8, amount * 1.2);
  return ret;
}

Resource lookup_resource(std::string name)
{
  name = no_caps( trim( name ) );
  for (int i = 0; i < RES_MAX; i++) {
    Resource ret = Resource(i);
    if (name == resource_name(ret)) {
      return ret;
    }
  }
  return RES_NULL;
}

std::string resource_name(Resource res)
{
  switch (res) {

    case RES_NULL:    return "NULL";

    case RES_GOLD:    return "gold";
    case RES_FOOD:    return "food";
    case RES_WOOD:    return "wood";
    case RES_STONE:   return "stone";

    case RES_FARMING: return "farming";
    case RES_MINING:  return "mining";
    case RES_HUNTING: return "hunting";
    case RES_LOGGING: return "logging";

    case RES_MAX:     return "BUG - RES_MAX";
    default:          return "BUG - Unnamed Resource";

  }
  return "BUG - Escaped resource_name() switch.";
}
