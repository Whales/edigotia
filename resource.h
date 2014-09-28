#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "color.h"
#include <string>

enum Resource
{
  RES_NULL = 0,

  RES_GOLD,
  RES_FOOD,
  RES_WOOD,
  RES_STONE,

// Meta-resources
  RES_FARMING,  // Can be any available crop
  RES_MINING,   // Available minerals, decided by the terrain
  RES_HUNTING,  // Available game, decided by the terrain
  RES_LOGGING,  // Produces wood based on terrain

  RES_MAX
};

Resource lookup_resource(std::string name);
std::string resource_name(Resource res);

enum Crop_type
{
  CROPTYPE_NULL = 0,
  CROPTYPE_FOOD,
  CROPTYPE_SPICE,
  CROPTYPE_DRUG,
  CROPTYPE_POISON,
  CROPTYPE_MATERIAL,
  CROPTYPE_OTHER,
  CROPTYPE_MAX
};

enum Crop
{
  CROP_NULL = 0,

// Food
  CROP_WHEAT,
  CROP_CABBAGE,
  CROP_GRAPES,
  CROP_MELON,
  CROP_RICE,
  CROP_CACTUS,

// Spices
  CROP_PEPPER,
  CROP_CINNAMON,

// Drugs
  CROP_TOBACCO,
  CROP_AMANITAS,
  CROP_AYAHUASCA,
  CROP_SPICEREED,

// Poisons
  CROP_DEATHCAP,
  CROP_VIPERVINE,
  CROP_SCORPICON,

// Materials
  CROP_COTTON,
  CROP_HEMP,

// Other
  CROP_HAY,

  CROP_MAX
};

enum Mineral
{
  MINERAL_NULL = 0,

  MINERAL_STONE,
  MINERAL_TIN,
  MINERAL_COPPER,
  MINERAL_IRON,
  MINERAL_GOLD,

  MINERAL_COAL,

  MINERAL_MAX
};

struct Crop_datum
{
  std::string name;
  int food;
  int percentage;
  Crop_type type;
  Crop_datum() { food = 0; percentage = 0; type = CROPTYPE_NULL; }

  nc_color get_color();
};

struct Mineral_datum
{
  std::string name;
  int percentage;

  Mineral_datum() { percentage = 0; }
};

struct Mineral_amount
{
  Mineral_amount() { type = MINERAL_NULL; amount = 0; }

  Mineral type;
  int amount;

  Mineral_amount randomize();
};

extern Crop_datum*    Crop_data   [CROP_MAX];
extern Mineral_datum* Mineral_data[MINERAL_MAX];

void init_crop_and_mineral_data();

struct Resource_amount
{
  Resource type;
  int amount;

  Resource_amount(Resource T, int A) : type (T), amount (A) {}
};

#endif
