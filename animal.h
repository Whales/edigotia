#ifndef _ANIMAL_H_
#define _ANIMAL_H_

#include "resource.h"
#include <string>
#include <vector>

/* NOTE:  This list should only include animals encountered as game (i.e. by
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
  ANIMAL_LION,
  ANIMAL_ELEPHANT,

// Fantasy creatures.  Generally very dangerous.
  ANIMAL_JELLY,
  ANIMAL_DIRE_WOLF,
  ANIMAL_MANTICORE,
  ANIMAL_UNICORN,
  ANIMAL_BASILISK,
  ANIMAL_WYVERN,

  ANIMAL_MAX
};

struct Animal_amount
{
  Animal_amount(Animal A = ANIMAL_NULL, int N = 0) :
    animal (A), amount (N) { }

  Animal animal;
  int amount;
};

struct Animal_datum
{
  Animal_datum();
  ~Animal_datum();

  std::string name;
  int uid;

// Placement data
  int min_temp, max_temp;         // Range of temperature we live in
  int min_altitude, max_altitude; // Range of altitude    we live in
  int min_rainfall, max_rainfall; // Range of rainfall    we live in
  int percentage;                 // Chance of placement

// Hunting/livestock data
  int danger;             // Dangerous to hunt? 10 = unarmed man, 20 = hunter
  int reproduction_rate;  // A percentage increase per year (range: 0 - 100)
  int hardiness;          // Livestock has a one-in-THIS chance of dying daily
                          // Use 0 if the livestock will never die randomly
  int food_killed;        // Food gained when killed (hunted or slaughtered)
  int food_livestock;     // Food gained daily, per 100 units, when livestock
  int food_eaten;         // Food eaten daily, per 100 units, when livestock
  bool carnivore;         // If false, this creature will eat hay if you have it
  int tameness;           // The chance that, when captured, becomes livestock
  std::vector<Resource_amount> resources_killed; // Res gained when killed
  std::vector<Resource_amount> resources_livestock; // Res per 100, gained daily

// TODO: Other flags.  For instance, it would be nice to have a flag noting that
//       it is a sin in the eyes of some gods to kill or "enslave" a unicorn.
};

// Defined in animal.cpp - searches for a partial match of the name, not caps
// sensitive, ignores whitespace
Animal search_for_animal(std::string name);

// Defined in animal_data.cpp
extern Animal_datum* Animal_data[ANIMAL_MAX];
void init_animal_data();

#endif
