#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "color.h"
#include <string>
#include <vector>

// INFINITE_RESOURCE is used e.g. when reporting how much stone is available in
// a mountain.
#define INFINITE_RESOURCE -99
/* HIDDEN_RESOURCE is used for Building::minerals_mined, which is of type
 * std::vector<Mineral_amount>.  When we open a new mine area, we look at the
 * underlying terrain, and add a Mineral_amount with a type corresponding to
 * each mineral available in the terrain, and a amount of HIDDEN_RESOURCE.  This
 * means that while the mineral is available, it isn't presented to the player
 * until the mine happens to find the mineral - at which point the amount is
 * changed from HIDDEN_RESOURCE to 0, meaning the player has the option to mine
 * that mineral (further increasing the amount).
 */
#define HIDDEN_RESOURCE   -999

enum Resource
{
  RES_NULL = 0,

// The Big Four
  RES_GOLD,
  RES_FOOD,
  RES_WOOD,
  RES_STONE,

// Component resources
  RES_FIBER,  // Anything than can become cloth; cotton, wool, etc
  RES_FUR,
  RES_LEATHER,

// Rare/monster resources
  RES_UNICORN_HORN,

// Meta-resources
  RES_FARMING,  // Can be any available crop
  RES_MINING,   // Available minerals, decided by the terrain
  RES_HUNTING,  // Available game, decided by the terrain
  RES_LOGGING,  // Produces wood based on terrain

  RES_MAX
};

Resource lookup_resource(std::string name);

struct Resource_datum
{
  std::string name;
  nc_color color;
  bool meta;  // True if it's not real; farming, mining, etc
};

struct Resource_amount
{
  Resource_amount(Resource R = RES_NULL, int A = 0) :
    type (R),
    amount (A)
    {}

  Resource type;
  int amount;

  bool is_infinite(); // True if amount == INFINITE_RESOURCE
};

enum Crop_type
{
  CROPTYPE_NULL = 0,
  CROPTYPE_FOOD,
  CROPTYPE_SPICE,
  CROPTYPE_DRUG,
  CROPTYPE_POISON,
  CROPTYPE_FIBER,
  CROPTYPE_OTHER,
  CROPTYPE_MAX
};

Crop_type lookup_crop_type(std::string name);
std::string crop_type_name(Crop_type type);
nc_color crop_type_color(Crop_type type);

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

  CROP_MAX
};

enum Mineral
{
  MINERAL_NULL = 0,

// Building materials
  MINERAL_STONE,

// Crafting metals
  MINERAL_TIN,
  MINERAL_COPPER,
  MINERAL_IRON,

// Rare valuables
  MINERAL_GEMS, // TODO: Should this be broken out into different TYPES of gems?
  MINERAL_GOLD,

// Fuels
  MINERAL_COAL,

  MINERAL_MAX
};

struct Crop_datum
{
  Crop_datum() { food = 0; percentage = 0; type = CROPTYPE_NULL; }

  nc_color get_color();

  std::string name;
  int food;
  int percentage;
  Crop_type type;

  std::vector<Resource_amount> bonus_resources;
};

struct Mineral_datum
{
  std::string name;
  int percentage;
  int value;  // Per 100 units
  nc_color color;
  bool hidden;

  Mineral_datum() { percentage = 0; color = c_ltgray; hidden = false; }
};

// These search for the name (trimmed & non-case-sensitive) in the data pools;
// partial matches work (e.g. a search for " Eat   " returns CROP_WHEAT)
Crop    search_for_crop   (std::string name);
Mineral search_for_mineral(std::string name);

struct Crop_amount
{
  Crop_amount(Crop T = CROP_NULL, int A = 0) :
    type (T),
    amount (A)
    {}

  Crop type;
  int amount;

  bool is_infinite(); // True if amount == INFINITE_RESOURCE
};

struct Mineral_amount
{
  Mineral_amount(Mineral T = MINERAL_NULL, int A = 0) :
    type (T),
    amount (A)
    {}

  Mineral type;
  int amount;

  bool is_infinite(); // True if amount == INFINITE_RESOURCE
// Returns this, with the amount set to 10%- 20% of the original.
  Mineral_amount make_small();
// Returns this, with the amount set to 80%-120% of the original.
  Mineral_amount randomize();
};

// Returns a description of the amount of mineral there.
// E.g. "Trace ", "Little ", "Some ", "Lots of ", "Tons of ", etc
std::string mineral_amount_ranking(Mineral_amount min_amt);

std::string trees_amount_ranking(int wood);

// Defined in resource_data.cpp
extern Resource_datum*  Resource_data [RES_MAX];
extern Crop_datum*      Crop_data     [CROP_MAX];
extern Mineral_datum*   Mineral_data  [MINERAL_MAX];
void init_resource_data();

#endif
