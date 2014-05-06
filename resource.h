#ifndef _RESOURCE_H_
#define _RESOURCE_H_

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

enum Crop
{
  CROP_NULL = 0,

// Food
  CROP_WHEAT,
  CROP_CORN,
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

// Building resources
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
  MINERAL_COPPER,
  MINERAL_IRON,
  MINERAL_GOLD,
  MINERAL_TIN,

  MINERAL_MAX
};

struct Crop_datum
{
  std::string name;
  int food;
};

struct Mineral_datum
{
  std::string name;
};

struct Resource_cost
{
  Resource type;
  int amount;

  Resource_cost(Resource T, int A) : type (T), amount (A) {}
};

#endif
