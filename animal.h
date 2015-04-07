#ifndef _ANIMAL_H_
#define _ANIMAL_H_

#include "resource.h"
#include <string>
#include <vector>
#include <map>

/* NOTE:  This enum should only include animals encountered as game (i.e. by
 *        hunter camps) or as livestock.  It should NOT include animals which
 *        have a defined location in the world map or move in the world map,
 *        which attack cities, or do anything else except exist as a passive
 *        encounters or as livestock.
 *        Those more interesting creatures should be defined in another file
 *        (TODO: Put note linking us to that file here!)
 */
enum Animal
{
  ANIMAL_NULL = 0,

// Standard livestock
  ANIMAL_CHICKEN,
  ANIMAL_PIG,
  ANIMAL_SHEEP,
  ANIMAL_COW,
  ANIMAL_HORSE,

// Standard game, mostly non-dangerous
  ANIMAL_HARE,
  ANIMAL_FOX,
  ANIMAL_BOAR,
  ANIMAL_DEER,

// Dangerous game - mostly more trouble than they're worth
  ANIMAL_BEAR,
  ANIMAL_WOLF,
  ANIMAL_TIGER,
  ANIMAL_LION,
  ANIMAL_ELEPHANT,

// Fantasy creatures.  Generally very dangerous.
  ANIMAL_JELLY,
  ANIMAL_MAN_EATING_PLANT,
  ANIMAL_DIRE_WOLF,
  ANIMAL_AKHLUT,      // Inuit legend - half orca, half wolf, amphibious
  ANIMAL_MANTICORE,
  ANIMAL_UNICORN,
  ANIMAL_BASILISK,
  ANIMAL_WYVERN,

  ANIMAL_MAX
};

enum Animal_action
{
  ANIMAL_ACT_NULL = 0,
  ANIMAL_ACT_FLEE,
  ANIMAL_ACT_KILL,
  ANIMAL_ACT_CAPTURE,
  ANIMAL_ACT_MAX
};

Animal_action lookup_animal_action(std::string name);
std::string animal_action_name(Animal_action action);
nc_color animal_action_color(Animal_action action);

struct Animal_amount
{
  Animal_amount(Animal A = ANIMAL_NULL, int N = 0) :
    type (A), amount (N) { }

  Animal type;
  int amount;

  bool is_infinite(); // True if amount == INFINITE_RESOURCE
// Returns this, with amount set to 10%-20% of the original.
  Animal_amount make_small();
// Returns this, with amount set to 80%-120% of the original.
  Animal_amount randomize();
};

struct Animal_datum
{
  Animal_datum();
  ~Animal_datum();

  std::string name;
  std::string name_plural;
  int uid;

// Placement data
  int min_temp, max_temp;         // Range of temperature we live in
  int min_altitude, max_altitude; // Range of altitude    we live in
  int min_rainfall, max_rainfall; // Range of rainfall    we live in
  int percentage;                 // Chance of placement

// Hunting/livestock data
  Animal_action default_action; // Default action taken when hunting
  int danger;             // Dangerous to hunt? 10 = unarmed man, 20 = hunter
  int difficulty;         // Hunting uses this many hunt points
  int hp;                 // Health points - how hard it is to kill
  int flee_chance;        // Chance for hunters to flee, if they choose to.
  int pack_chance;        // Chance that we'll appear in a pack.
  int max_pack_size;      // What's the biggest group we'll appear in?
  int reproduction_rate;  // Livestock has a one-in-THIS chance of birth daily
  int hardiness;          // Livestock has a one-in-THIS chance of dying daily
                          // Use 0 if the livestock will never die randomly
  int tameness;           // The chance that, when captured, becomes livestock
  int size;               // The amount of pasture space it uses; note that a
                          // pasture provides 500 space.

// Resource-related data
  int food_killed;        // Food gained when killed (hunted or slaughtered)
  int food_livestock;     // Food gained daily, per 100 units, when livestock
  bool carnivore;         // If false, this creature will eat hay if you have it
  std::vector<Resource_amount> resources_killed; // Res gained when killed
  std::vector<Resource_amount> resources_livestock; // Res per 100, gained daily

// TODO: Other flags.  For instance, it would be nice to have a flag noting that
//       it is a sin in the eyes of some gods to kill or "enslave" a unicorn.
};

// Defined in animal.cpp - searches for a partial match of the name, not caps
// sensitive, ignores whitespace
Animal search_for_animal(std::string name);
// Returns a description of the amount of the animal there.
// E.g. "A few", "A pack of", "A colony of", "A large colony of", etc
// If range is true, return the range (e.g. "301 - 750") not the text.
std::string animal_amount_ranking(int amt, bool range = false);

// Resturns a COLOR-CODED description of an animal's danger level.
// E.g. "Harmless", "Weak", "Mild", "Moderate", "High", "Deadly", "Nightmare"
std::string animal_danger_ranking(int danger);

// List the contents of the map with counts, punctuation, etc.
// E.g. "2 deer, a hare and 4 foxes"
std::string list_animals(std::map<Animal,int> animals, std::string verb = "");

// Defined in animal_data.cpp
extern Animal_datum* Animal_data[ANIMAL_MAX];
void init_animal_data();

#endif
