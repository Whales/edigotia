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

// Forged minerals
  RES_TIN,
  RES_COPPER,
  RES_IRON,

// Component resources
  RES_FIBER,  // Anything than can become cloth; cotton, wool, etc
  RES_FUR,
  RES_LEATHER,

// Rare/monster resources
  RES_UNICORN_HORN,

// Luxuries - these impart morale
  // Spices
  RES_SALT,
  RES_PEPPER,
  RES_CINNAMON,
  RES_CUMIN,
  RES_PAPRIKA,
  // Smokeables
  RES_TOBACCO,
  RES_CANNABIS,
  // Hallucinogens
  RES_AMANITAS,
  RES_AYAHUASCA,
  RES_SPICEREED,
  // Clothing
  RES_CLOTHING,
  RES_FUR_COATS,
  RES_LEATHER_COATS,
  // Housewares
  RES_WOOD_HOUSEWARES,
  RES_TIN_HOUSEWARES,
  RES_COPPER_HOUSEWARES,
  RES_IRON_HOUSEWARES,
  // Misc
  RES_FURNITURE,
  RES_JEWELRY,

// Non-luxury produced goods
  RES_BLANK_BOOK,

// Meta-resources
  RES_FARMING,  // Can be any available crop
  RES_MINING,   // Available minerals, decided by the terrain
  RES_HUNTING,  // Available game, decided by the terrain
  RES_LOGGING,  // Produces wood based on terrain

  RES_MAX
};

Resource lookup_resource(std::string name);

enum Luxury_type
{
  LUX_NULL = 0, // A valid option, if it's a unique luxury

  LUX_SPICE,    // Food spices.
  LUX_SMOKABLE, // A smokable soft drug
  LUX_HALLUCINOGEN,
  LUX_COATS,
  LUX_HOUSEWARES,

  LUX_MAX
};

// See resource.cpp for this stuff
Luxury_type lookup_luxury_type(std::string name);
std::string luxury_type_name(Luxury_type type);

struct Resource_datum
{
  Resource_datum(std::string N = "", nc_color C = c_white, int V = 0, int D = 0,
                 int M = 0, Luxury_type L = LUX_NULL, bool MT = false) :
    name (N), color (C), value (V), demand (D), morale (M), luxury_type (L),
    meta (MT) { }

  std::string name;
  nc_color color;

// The actual price we pay for a resource is determined by supply & demand, and
// the price a city sets for it.  But the value property is a basic guide and
// helps AI cities determine what crops to grow, for instance.
  int value;
  int demand; // Standard demand each turn per 100 citizens
  int morale; // Morale boost when the demand is met
  Luxury_type luxury_type;
  bool meta;  // True if it's not real; farming, mining, etc

  std::string description; // For help article
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
  CROP_CUMIN,
  CROP_PAPRIKA,

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
  MINERAL_SALT,
  MINERAL_GEMS, // TODO: Should this be broken out into different TYPES of gems?
  MINERAL_GOLD,

// Fuels
  MINERAL_COAL,

  MINERAL_MAX
};

struct Crop_datum
{
  Crop_datum()
  {
    food = 0;
    percentage = 0;
    type = CROPTYPE_NULL;
    min_temp = 0;
    max_temp = 100;
    min_altitude = 0;
    max_altitude = 100;
    min_rainfall = 0;
    max_rainfall = 100;
  }

  nc_color get_color();

  std::string name;
  int food;
  int percentage;
  int min_temp, max_temp;         // Range of temperature we grow in
  int min_altitude, max_altitude; // Range of altitude    we grow in
  int min_rainfall, max_rainfall; // Range of rainfall    we grow in
  Crop_type type;

  std::vector<Resource_amount> bonus_resources;

  std::string description;
};

struct Mineral_datum
{
  Mineral_datum() { percentage = 0; value = 0; color = c_ltgray; hidden = false;
                  }

  std::string name;
  int percentage;
  int value;  // Per 100 units
  nc_color color;
  bool hidden;

  std::string description;
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
