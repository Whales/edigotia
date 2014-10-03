#include "resource.h"
#include "stringfunc.h"
#include "rng.h"

bool Resource_amount::is_infinite()
{
  return (amount == INFINITE_RESOURCE);
}

bool Crop_amount::is_infinite()
{
  return (amount == INFINITE_RESOURCE);
}

bool Mineral_amount::is_infinite()
{
  return (amount == INFINITE_RESOURCE);
}

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

Crop_type lookup_crop_type(std::string name)
{
  name = no_caps( trim( name ) );
  for (int i = 0; i < CROPTYPE_MAX; i++) {
    Crop_type ret = Crop_type(i);
    if (no_caps(crop_type_name(ret)) == name) {
      return ret;
    }
  }
  return CROPTYPE_NULL;
}

std::string crop_type_name(Crop_type type)
{
  switch (type) {
    case CROPTYPE_NULL:     return "NULL";
    case CROPTYPE_FOOD:     return "food";
    case CROPTYPE_SPICE:    return "spice";
    case CROPTYPE_DRUG:     return "drug";
    case CROPTYPE_POISON:   return "poison";
    case CROPTYPE_MATERIAL: return "material";
    case CROPTYPE_OTHER:    return "other";
    case CROPTYPE_MAX:      return "BUG - CROPTYPE_MAX";
    default:                return "Unnamed Crop_type";
  }
  return "BUG - Escaped crop_type_name() switch";
}

nc_color crop_type_color(Crop_type type)
{
  switch (type) {
    case CROPTYPE_NULL:     return c_ltgray;
    case CROPTYPE_FOOD:     return c_ltgreen;
    case CROPTYPE_SPICE:    return c_yellow;
    case CROPTYPE_DRUG:     return c_magenta;
    case CROPTYPE_POISON:   return c_ltred;
    case CROPTYPE_MATERIAL: return c_ltblue;
    case CROPTYPE_OTHER:    return c_brown;
    case CROPTYPE_MAX:      return c_ltgray;
    default:                return c_ltgray;
  }
  return c_ltgray;
}

Crop search_for_crop(std::string name)
{
  name = no_caps( trim(name) );
  for (int i = 1; i < CROP_MAX; i++) {
    Crop ret = Crop(i);
    if (Crop_data[ret]->name.find(name) != std::string::npos) {
      return ret;
    }
  }
  return CROP_NULL;
}

Mineral search_for_mineral(std::string name)
{
  name = no_caps( trim(name) );
  for (int i = 1; i < MINERAL_MAX; i++) {
    Mineral ret = Mineral(i);
    if (Mineral_data[ret]->name.find(name) != std::string::npos) {
      return ret;
    }
  }
  return MINERAL_NULL;
}
