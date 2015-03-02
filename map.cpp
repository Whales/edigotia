#include "map.h"
#include "rng.h"
#include "geometry.h" // For direction
#include "window.h" // For debugmsg
#include "stringfunc.h" // for capitalize()
#include <sstream>

Map_tile::Map_tile()
{
  ter = TER_NULL;
}

Map_tile::~Map_tile()
{
}

std::string Map_tile::save_data()
{
  std::stringstream ret;
  ret << ter << " ";
  ret << wood << " ";

  ret << crops.size() << " ";
  for (int i = 0; i < crops.size(); i++) {
    ret << int(crops[i]) << " ";
  }

  ret << minerals.size() << " ";
  for (int i = 0; i < minerals.size(); i++) {
    ret << int(minerals[i].type) << " " << minerals[i].amount << " ";
  }

  ret << animals.size() << " ";
  for (int i = 0; i < animals.size(); i++) {
    ret << int(animals[i].type) << " " << animals[i].amount << " ";
  }

  return ret.str();
}

bool Map_tile::load_data(std::istream& data)
{
  int tmpter;
  data >> tmpter;
  if (tmpter <= 0 || tmpter >= TER_MAX) {
    debugmsg("Map_tile loaded Terrain_type %d (range is 1 to %d).",
             tmpter, TER_MAX - 1);
    return false;
  }
  ter = Terrain_type(tmpter);

  data >> wood;

  int num_crops;
  data >> num_crops;
  for (int i = 0; i < num_crops; i++) {
    int tmpcrop;
    data >> tmpcrop;
    if (tmpcrop <= 0 || tmpcrop >= CROP_MAX) {
      debugmsg("Map_tile loaded crop %d (range is 1 to %d).",
               tmpcrop, CROP_MAX - 1);
      return false;
    }
    crops.push_back( Crop(tmpcrop) );
  }

  int num_minerals;
  data >> num_minerals;
  for (int i = 0; i < num_minerals; i++) {
    int tmpmineral, tmpnum;
    data >> tmpmineral >> tmpnum;
    if (tmpmineral <= 0 || tmpmineral >= MINERAL_MAX) {
      debugmsg("Map_tile loaded mineral %d (range is 1 to %d).",
               tmpmineral, MINERAL_MAX - 1);
      return false;
    }
    minerals.push_back( Mineral_amount( Mineral(tmpmineral), tmpnum ) );
  }

  int num_animals;
  data >> num_animals;
  for (int i = 0; i < num_animals; i++) {
    int tmpanimal, tmpnum;
    data >> tmpanimal >> tmpnum;
    if (tmpanimal <= 0 || tmpanimal >= ANIMAL_MAX) {
      debugmsg("Map_tile loaded animal %d (range is 1 to %d).",
               tmpanimal, ANIMAL_MAX - 1);
      return false;
    }
    animals.push_back( Animal_amount( Animal(tmpanimal), tmpnum ) );
  }

  return true;
}

Terrain_datum* Map_tile::get_terrain_datum()
{
  return Terrain_data[ter];
}

std::string Map_tile::get_terrain_name()
{
  return Terrain_data[ter]->name;
}

glyph Map_tile::get_glyph()
{
  return Terrain_data[ter]->symbol;
}

std::string Map_tile::get_info()
{
  std::stringstream ret;

  ret << "<c=white>" << Terrain_data[ter]->name << "<c=/>" << std::endl;

  if (!crops.empty()) {
    ret << "<c=green>";
    ret << "Crops: " << get_crop_info() << std::endl;
    ret << "Farm Output: " << get_farmability() << "%%%%";
    ret << "<c=/>";
  } else {
    ret << "<c=red>No crops.<c=/>";
  }

  if (wood > 0) {
    ret << std::endl;
    ret << "<c=brown>";
    ret << capitalize(trees_amount_ranking(wood)) << " trees";
    ret << "<c=/>";
  }

  return ret.str();
}

std::string Map_tile::get_crop_info()
{
  if (crops.empty()) {
    return "None";
  }
  std::stringstream ret;
  for (int i = 0; i < crops.size(); i++) {
    ret << Crop_data[ crops[i] ]->name;
    if (i < crops.size() - 1) {
      ret << " ";
    }
  }
  return ret.str();
}

std::string Map_tile::get_animals_info()
{
  if (animals.empty()) {
    return "None";
  }
  std::stringstream ret;
  for (int i = 0; i < animals.size(); i++) {
    ret << capitalize( animal_amount_ranking( animals[i].amount ) ) << " " <<
           Animal_data[ animals[i].type ]->name_plural;
    if (i < animals.size() - 1) {
      ret << "," << std::endl;
    }
  }
  return ret.str();
}

int Map_tile::get_farmability()
{
  return Terrain_data[ter]->farm_percent;
}

// prioritize_food defaults to true
Crop Map_tile::get_best_crop(bool prioritize_food)
{
  if (!can_build(AREA_FARM)) {
    return CROP_NULL;
  }
  if (crops.empty()) {
    return CROP_NULL;
  }
// Value of each crop for food and resource
  std::vector<int> food_value, res_value;

  for (int i = 0; i < crops.size(); i++) {
    Crop crop = crops[i];
    Crop_datum* crop_dat = Crop_data[crop];
    int total_food_value = Resource_data[RES_FOOD]->value;
    food_value.push_back(crop_dat->food * total_food_value * get_farmability());
// Find the total value of the resources this crop produces
    int total_res_value = 0;
    for (int n = 0; n < crop_dat->bonus_resources.size(); n++) {
      Resource_amount res_amt = crop_dat->bonus_resources[n];
      Resource_datum* res_dat = Resource_data[res_amt.type];
      total_res_value += res_amt.amount * get_farmability() * res_dat->value;
    }
    res_value.push_back( total_res_value );
  }

// Now loop through and find the best one!
  int best_value = 0,  best_food_value = 0,
      best_index = -1, best_food_index = -1;
  for (int i = 0; i < crops.size(); i++) {
    int this_value = food_value[i] + res_value[i];
    int this_food_value = food_value[i];
    if (this_value > best_value) {
      best_value = this_value;
      best_index = i;
    }
    if (this_food_value > best_food_value) {
      best_food_value = this_food_value;
      best_food_index = -1;
    }
  }

// If we're prioritizing food, try to use best_food_index first.
  if (prioritize_food && best_food_index != -1) {
    return crops[best_food_index];
  }
  if (best_index == -1) {
// No useful crops here!
    return CROP_NULL;
  }
  return crops[best_index];
}

int Map_tile::get_max_food_output()
{
  if (!can_build(AREA_FARM)) {
    return 0;
  }
// First, find our best crop.
  int best_food = 0;
  for (int i = 0; i < crops.size(); i++) {
    Crop crop = crops[i];
    Crop_datum* crop_dat = Crop_data[crop];
    if (crop_dat->food > best_food) {
      best_food = crop_dat->food;
    }
  }
  if (best_food == 0) { // No crops here produce food!
    return 0;
  }

  int farmability = get_farmability();
// We don't divide by 10000 at this point, in order to avoid rounding errors.
  return (best_food * farmability);
}

// Here the crop "value" is the 100 - Crop_datum's percentage value.
int Map_tile::get_resource_crop_output()
{
  if (!can_build(AREA_FARM)) {
    return 0;
  }

// Go through all crops here, and find the one that produces the most high-value
// resource.
  int best_value = 0;
  for (int i = 0; i < crops.size(); i++) {
    Crop crop = crops[i];
    Crop_datum* crop_dat = Crop_data[crop];
// Find the resource associated with this crop with the highest value
    int best_res_value = 0;
    for (int n = 0; n < crop_dat->bonus_resources.size(); n++) {
      Resource res = crop_dat->bonus_resources[n].type;
      Resource_datum* res_dat = Resource_data[res];
      int res_value = crop_dat->bonus_resources[n].amount * res_dat->value;
      if (res_value > best_res_value) {
        best_res_value = res_value;
      }
    }
// Now compare the best resource that crop produces with the best we've found so
// far.
    if (best_res_value > best_value) {
      best_value = best_res_value;
    }
  }

// Bonus resources ignore farmability, so just return that value.
  return best_value;
}

int Map_tile::get_avg_hunting_output()
{
  if (!can_build(AREA_HUNTING_CAMP)) {
    return 0;
  }
// Find the sums and divide by the total count
  int food_sum = 0;
// Divisor should include terrain's hunting difficulty (chance of no animals)
  int divisor = get_terrain_datum()->hunting_difficulty;
  for (int i = 0; i < animals.size(); i++) {
    Animal_datum* animal_dat = Animal_data[ animals[i].type ];
    int food = animal_dat->food_killed;
    int count = animals[i].amount;

// Increase food if the animal appears in packs.
    if (animal_dat->pack_chance > 0 && animal_dat->max_pack_size > 1) {
      int pack_size = animal_dat->max_pack_size / 2;
// Only multiply by pack_size if it represents an increase
// Add the chance of extra food we have.
      food += (food * (pack_size - 1) * animal_dat->pack_chance) / 100;
    }

// Reduce food output if it's dangerous.  TODO: Tweak?
// This uses danger / 60 as a percentage to lose; e.g. danger 6 => lose 10%
    if (animal_dat->danger > 0) {
      food = food - (food * animal_dat->danger) / 60;
    }

    food_sum += food * count;
    divisor += count;

  }
  if (divisor == 0) {
    return 0;
  }
  return (food_sum / divisor);
}

bool Map_tile::can_build(Area_type area)
{
  Terrain_datum* ter_dat = get_terrain_datum();
  if (!ter_dat) {
    return false;
  }

  for (int i = 0; i < ter_dat->buildable_areas.size(); i++) {
    if (ter_dat->buildable_areas[i] == area) {
      return true;
    }
  }
  return false;
}

Mineral_amount* Map_tile::lookup_mineral(Mineral mineral)
{
  for (int i = 0; i < minerals.size(); i++) {
    if (minerals[i].type == mineral) {
      return &(minerals[i]);
    }
  }
  return NULL;
}

int Map_tile::get_mineral_amount(Mineral mineral)
{
  for (int i = 0; i < minerals.size(); i++) {
    if (minerals[i].type == mineral) {
      return minerals[i].amount;
    }
  }
  return 0;
}

bool Map_tile::remove_mineral(Mineral mineral)
{
  for (int i = 0; i < minerals.size(); i++) {
    if (minerals[i].type == mineral) {
      minerals.erase( minerals.begin() + i );
      return true;
    }
  }
  return false;
}

Animal Map_tile::choose_hunt_animal(int skill_level)
{
// Figure out the total population, including the difficulty of the terrain
  int total_pop = get_terrain_datum()->hunting_difficulty;
  if (total_pop > 0) {
// Modify the odds of finding nothing using our skill level
// Triple or double for poor skill; 75% or 60% for good skill.
    total_pop = (total_pop * 3) / skill_level;
  }
  for (int i = 0; i < animals.size(); i++) {
    total_pop += animals[i].amount;
  }

  int roll = rng(1, total_pop);
// Generate a random number, and subtract populations til we hit 0
  for (int i = 0; i < animals.size(); i++) {
    roll -= animals[i].amount;
    if (roll <= 0) {
      return animals[i].type;
    }
  }
// At this point, if we haven't hit zero it's due to the terrain difficulty.
// Which means we weren't able to catch anything!
  return ANIMAL_NULL;
}

Animal Map_tile::get_best_hunt_animal(int hunter_level)
{
  int best_food = 0, best_index = -1, best_difficulty = 999999;
  for (int i = 0; i < animals.size(); i++) {
    Animal_datum* ani_dat = Animal_data[ animals[i].type ];
    if (ani_dat->difficulty == 0) {
      debugmsg("%s has difficulty 0!", ani_dat->name.c_str());
    } else {
      int num_caught = hunter_level / ani_dat->difficulty;
      int remainder  = hunter_level % ani_dat->difficulty;
      int food = num_caught * ani_dat->food_killed +
                 (ani_dat->food_killed * remainder) / ani_dat->difficulty;

// On a tie, choose whichever has lower difficulty.
      if (food > best_food ||
          (food == best_food && ani_dat->difficulty < best_difficulty)) {
        best_food = food;
        best_difficulty = ani_dat->difficulty;
        best_index = i;
      }
    }
  }
  if (best_index == -1) { // No food-producing animals here!
    return ANIMAL_NULL;
  }
  return animals[best_index].type;
}

int Map_tile::get_animal_population(Animal animal)
{
  for (int i = 0; i < animals.size(); i++) {
    if (animals[i].type == animal) {
      return animals[i].amount;
    }
  }
  return 0;
}

void Map_tile::clear_wood()
{
  Terrain_datum* ter_dat = get_terrain_datum();
  if (ter_dat->wood_cleared_type == TER_NULL) { // Doesn't change.
    return;
  }
  ter = ter_dat->wood_cleared_type;
  wood = 0;
// No other changes necessary; we retain the crops/minerals of our previous type
}

City_map::City_map()
{
}

City_map::~City_map()
{
}

void City_map::generate(Map_type type,
                        std::vector<Crop> world_crops,
                        std::vector<Mineral> world_minerals,
                        std::vector<Animal> world_animals,
                        Direction coast,
                        Direction_full river_start, Direction_full river_end)
{
  int chance[TER_MAX];  // Relative chance for each terrain type to appear.
  int total_chance = 0;
  for (int i = 0; i < TER_MAX; i++) {
    chance[i] = 0;
  }
  switch (type) {
    case MAP_TUNDRA:
      chance[TER_TUNDRA]        = 100;
      chance[TER_ROCKY]         =   1;
      chance[TER_HILL]          =   5;
      chance[TER_ICY_HILL]      =   2;
      chance[TER_MOUNTAIN]      =   5;
      chance[TER_ICY_MOUNTAIN]  =   2;
      chance[TER_FOREST]        =   5;
      break;

    case MAP_PLAINS:
      chance[TER_FIELD]     = 75;
      chance[TER_ROCKY]     = 14;
      chance[TER_HILL]      =  8;
      chance[TER_MOUNTAIN]  =  2;
      chance[TER_FOREST]    =  9;
      chance[TER_SWAMP]     =  1;
      break;

    case MAP_WASTELAND:
      chance[TER_FIELD]     =  8;
      chance[TER_ROCKY]     = 60;
      chance[TER_HILL]      = 14;
      chance[TER_MOUNTAIN]  =  1;
      chance[TER_FOREST]    =  2;
      chance[TER_DESERT]    =  6;
      break;

    case MAP_FOREST:
      chance[TER_FIELD]     =  5;
      chance[TER_ROCKY]     =  4;
      chance[TER_HILL]      =  2;
      chance[TER_MOUNTAIN]  =  1;
      chance[TER_FOREST]    = 45;
      chance[TER_SWAMP]     =  6;
      break;

    case MAP_FOOTHILLS:
      chance[TER_FIELD]     =  8;
      chance[TER_ROCKY]     = 20;
      chance[TER_HILL]      = 50;
      chance[TER_MOUNTAIN]  =  8;
      chance[TER_FOREST]    =  4;
      chance[TER_DESERT]    =  1;
      break;

    case MAP_ICY_FOOTHILLS:
      chance[TER_TUNDRA]       =  60;
      chance[TER_ROCKY]        =  10;
      chance[TER_HILL]         =  40;
      chance[TER_ICY_HILL]     = 100;
      chance[TER_MOUNTAIN]     =   8;
      chance[TER_ICY_MOUNTAIN] =  22;
      chance[TER_FOREST]       =   1;
      break;

    case MAP_MOUNTAINOUS:
      chance[TER_FIELD]     =  1;
      chance[TER_ROCKY]     = 10;
      chance[TER_HILL]      = 24;
      chance[TER_MOUNTAIN]  = 60;
      chance[TER_FOREST]    =  3;
      chance[TER_DESERT]    =  1;
      break;

    case MAP_ICY_MOUNTAIN:
      chance[TER_TUNDRA]       =   7;
      chance[TER_ROCKY]        =   3;
      chance[TER_HILL]         =  30;
      chance[TER_ICY_HILL]     =  60;
      chance[TER_MOUNTAIN]     =  20;
      chance[TER_ICY_MOUNTAIN] = 200;
      break;

    case MAP_BASIN:
      chance[TER_FIELD]     = 80;
      chance[TER_ROCKY]     =  4;
      chance[TER_HILL]      =  2;
      chance[TER_FOREST]    =  9;
      chance[TER_SWAMP]     =  7;
// We'll stick in a river later
      break;

    case MAP_CANYON:
      chance[TER_FIELD]     =  2;
      chance[TER_ROCKY]     = 12;
      chance[TER_HILL]      = 24;
      chance[TER_MOUNTAIN]  = 60;
      chance[TER_FOREST]    =  8;
      chance[TER_SWAMP]     =  1;
// We'll stick in a river later
      break;

    case MAP_GLACIER:
      chance[TER_TUNDRA]       = 60;
      chance[TER_ROCKY]        = 10;
      chance[TER_HILL]         =  3;
      chance[TER_ICY_HILL]     =  3;
      chance[TER_MOUNTAIN]     =  3;
      chance[TER_ICY_MOUNTAIN] =  3;
      chance[TER_FOREST]       =  5;
// We'll stick in a glacier later
      break;

    case MAP_COASTAL:
      chance[TER_FIELD]     = 30;
      chance[TER_ROCKY]     = 10;
      chance[TER_HILL]      =  2;
      chance[TER_FOREST]    =  6;
      chance[TER_SWAMP]     =  5;
      break;

    case MAP_SWAMP:
      chance[TER_FIELD]     =  4;
      chance[TER_ROCKY]     =  6;
      chance[TER_HILL]      =  4;
      chance[TER_MOUNTAIN]  =  1;
      chance[TER_FOREST]    =  7;
      chance[TER_SWAMP]     = 40;
      chance[TER_JUNGLE]    =  2;
      break;

    case MAP_JUNGLE:
      chance[TER_FIELD]     =   2;
      chance[TER_ROCKY]     =   2;
      chance[TER_HILL]      =   5;
      chance[TER_MOUNTAIN]  =   1;
      chance[TER_FOREST]    =   4;
      chance[TER_SWAMP]     =  10;
      chance[TER_JUNGLE]    = 120;
      break;

    case MAP_DESERT:
      chance[TER_FIELD]     =  1;
      chance[TER_ROCKY]     = 15;
      chance[TER_HILL]      =  2;
      chance[TER_MOUNTAIN]  =  4;
      chance[TER_DESERT]    = 60;
      break;

    case MAP_OCEAN:
      chance[TER_FIELD]     =  1;
      chance[TER_MOUNTAIN]  =  1;
      chance[TER_DESERT]    =  2;
      chance[TER_OCEAN]     = 80;
      break;

    case MAP_ICECAP:
      chance[TER_ICE]       = 40;
      chance[TER_TUNDRA]    =  5;
      chance[TER_FOREST]    =  1;
  }

  if (type != MAP_OCEAN && coast != DIR_NULL) {  // Alter chances a bit.
    chance[TER_TUNDRA]        *= 1.5;
    chance[TER_ICY_HILL]      *=  .7;
    chance[TER_ICY_MOUNTAIN]  *=  .3;
    chance[TER_FIELD]         *= 1.5;
    chance[TER_ROCKY]         *=  .8;
    chance[TER_HILL]          *=  .6;
    chance[TER_MOUNTAIN]      *=  .2;
    chance[TER_FOREST]        *= 1.3;
    chance[TER_SWAMP]         *= 1.2;
    chance[TER_JUNGLE]        *= 1.2;
  }

  for (int i = 0; i < TER_MAX; i++) {
    total_chance += chance[i];
  }

  for (int x = 0; x < CITY_MAP_SIZE; x++) {
    for (int y = 0; y < CITY_MAP_SIZE; y++) {
      int sel = rng(1, total_chance);
      int index = 0;
      while (sel > 0) {
        sel -= chance[index];
        if (sel > 0) {
          index++;
        }
      }
      tiles[x][y].ter = Terrain_type(index);
    }
  }

// Now fix up coastal / basin
  if (type != MAP_OCEAN && coast != DIR_NULL) {
    int x = 0, x2 = 1, x3 = 2, x4 = 3;
    int y = 0, y2 = 1, y3 = 2, y4 = 3;
    if (coast == DIR_SOUTH) {
      y  = CITY_MAP_SIZE - 1;
      y2 = CITY_MAP_SIZE - 2;
      y3 = CITY_MAP_SIZE - 3;
      y4 = CITY_MAP_SIZE - 4;
    } else if (coast == DIR_EAST) {
      x  = CITY_MAP_SIZE - 1;
      x2 = CITY_MAP_SIZE - 2;
      x3 = CITY_MAP_SIZE - 3;
      x4 = CITY_MAP_SIZE - 4;
    }
    for (int i = 0; i < CITY_MAP_SIZE; i++) {
      if (coast == DIR_NORTH || coast == DIR_SOUTH) {
        x = i;
        tiles[x][y2].ter = TER_OCEAN;
        if (one_in(5)) {
          tiles[x][y3].ter = TER_OCEAN;
          tiles[x][y4].ter = TER_OCEAN;
        } else if (one_in(3)) {
          tiles[x][y3].ter = TER_OCEAN;
        }
      } else {
        y = i;
        tiles[x2][y].ter = TER_OCEAN;
        if (one_in(5)) {
          tiles[x3][y].ter = TER_OCEAN;
          tiles[x4][y].ter = TER_OCEAN;
        } if (one_in(3)) {
          tiles[x3][y].ter = TER_OCEAN;
        }
      }
      tiles[x][y].ter = TER_OCEAN;
    }

  }
  if (Map_type_data[type]->is_river) {
// TODO: Do we need to specify which direction the river travels?
//       For now, it's just northwest => southeast
    Terrain_type river_type = TER_RIVER;
    if (type == MAP_GLACIER) {
      river_type = TER_GLACIER;
    }
    int x = 0, y = 0;
    int dx = 1, dy = 1;
    int mid = CITY_MAP_SIZE / 2, end = CITY_MAP_SIZE - 1;
    bool lake = false;
    switch (river_start) {
      case DIRFULL_NORTHWEST: x = 0;    y = 0;    break;
      case DIRFULL_WEST:      x = 0;    y = mid;  break;
      case DIRFULL_NORTH:     x = mid;  y = 0;    break;
      case DIRFULL_SOUTHWEST: x = 0;    y = end;  break;
      case DIRFULL_NORTHEAST: x = end;  y = 0;    break;

/* Special case!  There's a river coming in but nowhere for it to go... that
 * makes this a LAKE!  COOOOOOL.  So stick a blob of lake somewhere and send our
 * river towards it.
 */
      case DIRFULL_NULL:
        lake = true;
// We need to set x/y based on river_end, and treat that as the start.
        switch (river_end) {
          case DIRFULL_SOUTHEAST: x = end;  y = end;  break;
          case DIRFULL_SOUTH:     x = mid;  y = end;  break;
          case DIRFULL_EAST:      x = end;  y = mid;  break;
          case DIRFULL_SOUTHWEST: x =   0;  y = end;  break;
          case DIRFULL_NORTHEAST: x = end;  y =   0;  break;

// Not a lake after all! :(  Should never happen.
          case DIRFULL_NULL:
            lake = false;
            break;
        }
        break;
    }

// Figure out what direction to move in based on where the river should end.
    switch (river_end) {
      case DIRFULL_SOUTHEAST: dx =  1;  dy =  1;  break;
      case DIRFULL_SOUTH:     dx =  0;  dy =  1;  break;
      case DIRFULL_EAST:      dx =  1;  dy =  0;  break;
      case DIRFULL_SOUTHWEST: dx = -1;  dy =  1;  break;
      case DIRFULL_NORTHEAST: dx =  1;  dy = -1;  break;

/* Special case!  There's a river coming in but nowhere for it to go... that
 * makes this a LAKE!  COOOOOOL.  So stick a blob of lake somewhere and send our
 * river towards it.
 */
      case DIRFULL_NULL:
        lake = true;
        break;
    }

// If x or y were set to the middle of the side, we can actually randomize their
// starting position a little bit.
    if (x == mid) {
      x += rng(0 - CITY_MAP_SIZE / 3, CITY_MAP_SIZE / 3);
    }
    if (y == mid) {
      y += rng(0 - CITY_MAP_SIZE / 3, CITY_MAP_SIZE / 3);
    }

    if (lake) {
      int lake_x = rng(1, CITY_MAP_SIZE - 3),
          lake_y = rng(1, CITY_MAP_SIZE - 3);
// Store the center tile in a buffer just in case the lake overwrites it
      Terrain_type buffer = tiles[CITY_MAP_SIZE / 2][CITY_MAP_SIZE / 2].ter;
      tiles[lake_x    ][lake_y    ].ter = TER_LAKE;
      tiles[lake_x + 1][lake_y    ].ter = TER_LAKE;
      tiles[lake_x    ][lake_y + 1].ter = TER_LAKE;
      tiles[lake_x + 1][lake_y + 1].ter = TER_LAKE;
      tiles[CITY_MAP_SIZE / 2][CITY_MAP_SIZE / 2].ter = buffer;
      while (x < lake_x || x > lake_x + 1 || y < lake_y || y > lake_y + 1) {
        if (tiles[x][y].ter != TER_OCEAN) {
          tiles[x][y].ter = river_type;
        }
        if (one_in(2)) {
          if (x < lake_x) {
            x++;
          } else if (x > lake_x + 1) {
            x--;
          }
        } else {
          if (y < lake_y) {
            y++;
          } else if (y > lake_y + 1) {
            y--;
          }
        }
      }

    } else { // Not a lake BOOOOO GIMME A LAKE

      while (x >= 0 && y >= 0 && x < CITY_MAP_SIZE && y < CITY_MAP_SIZE) {
// Don't replace ocean with river!
        if (tiles[x][y].ter != TER_OCEAN) {
          tiles[x][y].ter = river_type;
        }
        if (one_in(2)) {
          if (dx == 0) {
            if (x == 0) {
              x++;
            } else if (x == CITY_MAP_SIZE - 1) {
              x--;
            } else {  // Random move
              x += (one_in(2) ? 1 : -1);
            }
          } else {
            x += dx;
          }
        } else {
          if (dy == 0) {
            if (y == 0) {
              y++;
            } else if (y == CITY_MAP_SIZE - 1) {
              y--;
            } else {  // Random move
              y += (one_in(2) ? 1 : -1);
            }
          } else {
            y += dy;
          }
        } // End of "move x or y?" block
      } // while (x >= 0 && y >= 0 && x < CITY_MAP_SIZE && y < CITY_MAP_SIZE)
    } // (!lake)

  } else if (type == MAP_OCEAN) {
// Obviously, not every ocean tile is going to have a prominent center isle;
// however since this is specifically for cities on ocean tiles, let's always
// give an island.
// TODO: Except if we have mermen, don't do this?
    for (int x = CITY_MAP_SIZE / 2 - 2; x <= CITY_MAP_SIZE / 2 + 2; x++) {
      for (int y = CITY_MAP_SIZE / 2 - 2; y <= CITY_MAP_SIZE / 2 + 2; y++) {
        if (one_in(4)) {
          tiles[x][y].ter = TER_DESERT;
        } else if (one_in(5)) {
          tiles[x][y].ter = TER_ROCKY;
        } else if ((x == CITY_MAP_SIZE / 2 && y == CITY_MAP_SIZE / 2) ||
                   one_in(2)) {
          tiles[x][y].ter = TER_FIELD;
        }
      }
    }
  }

// Set up wood content.
  for (int x = 0; x < CITY_MAP_SIZE; x++) {
    for (int y = 0; y < CITY_MAP_SIZE; y++) {
      Terrain_datum* ter_dat = tiles[x][y].get_terrain_datum();
      tiles[x][y].wood = rng(ter_dat->wood_min, ter_dat->wood_max);
    }
  }
  
// Now place some resources...
  for (int x = 0; x < CITY_MAP_SIZE; x++) {
    for (int y = 0; y < CITY_MAP_SIZE; y++) {
/* For safety's sake - sometimes if we examine a map, decide not to build a city
 * there, then return to the same map, this becomes relevent because the old
 * resources are still there from the first generation.
 */
      tiles[x][y].crops.clear();
      tiles[x][y].minerals.clear();
      tiles[x][y].animals.clear();
      Terrain_datum* ter_dat = Terrain_data[ tiles[x][y].ter ];

// Crops
      for (int i = 0; i < ter_dat->crops.size(); i++) {
// Check if the world map assigned us this crop.
        Crop crop = ter_dat->crops[i];
        bool crop_assigned = false;
        for (int n = 0; !crop_assigned && n < world_crops.size(); n++) {
          if (world_crops[n] == crop) {
            crop_assigned = true;
          }
        }
// Only assign the crop if we got it from the world map, or on a small chance
        Crop_datum* crop_dat = Crop_data[crop];
        if (!crop_dat) {
          debugmsg("NULL crop_dat (crop %d)", crop);
        }
        if (crop_assigned ||
            (rng(1, 100) <= crop_dat->percentage &&
             rng(1, 100) <= crop_dat->percentage &&
             rng(1, 100) <= crop_dat->percentage && one_in(4))) {
          tiles[x][y].crops.push_back(crop);
        }
      }

// Minerals
      for (int i = 0; i < ter_dat->minerals.size(); i++) {
        Mineral_amount min_amount = ter_dat->minerals[i];
        Mineral mineral = min_amount.type;
// Check if the world map assigned us this mineral.
        bool mineral_assigned = false;
        for (int n = 0; !mineral_assigned && n < world_minerals.size(); n++) {
          if (world_minerals[n] == mineral) {
            mineral_assigned = true;
          }
        }
        Mineral_datum* mineral_dat = Mineral_data[mineral];
        if (!mineral_dat) {
          debugmsg("NULL mineral_dat (mineral %d)", mineral);
        }
// Some terrains have an infinite amount of a mineral (e.g. hills & stone)
// In this case, we leave it unchanged (obviously) and always assign it (obvs)
        if (min_amount.is_infinite()) {
          tiles[x][y].minerals.push_back( min_amount );
        } else if (mineral_assigned) {
          tiles[x][y].minerals.push_back( min_amount.randomize() );
        } else if (rng(1, 150) <= mineral_dat->percentage && one_in(2)) {
          tiles[x][y].minerals.push_back( min_amount.make_small() );
        }
      }

// Animals
      for (int i = 0; i < ter_dat->animals.size(); i++) {
        Animal_amount ani_amount = ter_dat->animals[i];
        Animal animal = ani_amount.type;
// Check if the world map assigned us this animal.
        bool animal_assigned = false;
        for (int n = 0; !animal_assigned && n < world_animals.size(); n++) {
          if (world_animals[n] == animal) {
            animal_assigned = true;
          }
        }
        Animal_datum* animal_dat = Animal_data[animal];
        if (!animal_dat) {
          debugmsg("NULL animal_dat (animal %d)", animal);
        }
/* Some terrains have an infinite amount of a animal; always copy it over.
 * This is copied from minerals above; should we NOT do this?  It doesn't seem
 * likely to ever occur.
 */
        if (ani_amount.is_infinite()) {
          tiles[x][y].animals.push_back( ani_amount );
        } else if (animal_assigned) {
          tiles[x][y].animals.push_back( ani_amount.randomize() );
        } else if (rng(1, 1000) <= animal_dat->percentage) {
          tiles[x][y].animals.push_back( ani_amount.make_small() );
        }
      }

    }
  }
}

std::string City_map::save_data()
{
  std::stringstream ret;

  for (int x = 0; x < CITY_MAP_SIZE; x++) {
    for (int y = 0; y < CITY_MAP_SIZE; y++) {
      ret << tiles[x][y].save_data() << " ";
    }
  }
  ret << std::endl;

  ret << bonus_crops.size() << " ";
  for (int i = 0; i < bonus_crops.size(); i++) {
    ret << int(bonus_crops[i]) << " ";
  }
  ret << std::endl;

  ret << bonus_minerals.size() << " ";
  for (int i = 0; i < bonus_minerals.size(); i++) {
    ret << int(bonus_minerals[i]) << " ";
  }
  ret << std::endl;

  return ret.str();
}

bool City_map::load_data(std::istream& data)
{
  for (int x = 0; x < CITY_MAP_SIZE; x++) {
    for (int y = 0; y < CITY_MAP_SIZE; y++) {
      if (!tiles[x][y].load_data(data)) {
        debugmsg("City_map failed to load tile [%d:%d].", x, y);
        return false;
      }
    }
  }

  int num_crops;
  data >> num_crops;
  for (int i = 0; i < num_crops; i++) {
    int tmpcrop;
    data >> tmpcrop;
    if (tmpcrop <= 0 || tmpcrop >= CROP_MAX) {
      debugmsg("City_map loaded crop %d (range is 1 to %d.",
               tmpcrop, CROP_MAX - 1);
      return false;
    }
    bonus_crops.push_back( Crop(tmpcrop) );
  }

  int num_minerals;
  data >> num_minerals;
  for (int i = 0; i < num_minerals; i++) {
    int tmpmineral;
    data >> tmpmineral;
    if (tmpmineral <= 0 || tmpmineral >= MINERAL_MAX) {
      debugmsg("City_map loaded mineral %d (range is 1 to %d.",
               tmpmineral, MINERAL_MAX - 1);
      return false;
    }
    bonus_minerals.push_back( Mineral(tmpmineral) );
  }

  return true;
}

Map_tile* City_map::get_tile(Point p)
{
  return get_tile(p.x, p.y);
}

Map_tile* City_map::get_tile(int x, int y)
{
  if (is_oob(x, y)) {
    return NULL;
  }
  return &(tiles[x][y]);
}

std::string City_map::get_resource_info(Point p)
{
  return get_resource_info(p.x, p.y);
}

std::string City_map::get_resource_info(int x, int y)
{
  if (is_oob(x, y)) {
    return std::string();
  }

  std::stringstream ret;
  return ret.str();
}

Terrain_datum* City_map::get_terrain_datum(Point p)
{
  return get_terrain_datum(p.x, p.y);
}

Terrain_datum* City_map::get_terrain_datum(int x, int y)
{
  if (is_oob(x, y)) {
    return Terrain_data[TER_NULL]; // Better/safer than returning NULL
  }
  return tiles[x][y].get_terrain_datum();
}

std::string City_map::get_terrain_name(Point p)
{
  return get_terrain_name(p.x, p.y);
}

std::string City_map::get_terrain_name(int x, int y)
{
  if (is_oob(x, y)) {
    return "Out of bounds";
  }
  return tiles[x][y].get_terrain_name();
}

glyph City_map::get_glyph(Point p)
{
  return get_glyph(p.x, p.y);
}

glyph City_map::get_glyph(int x, int y)
{
  if (is_oob(x, y)) {
    return glyph();
  }

  return tiles[x][y].get_glyph();
}

std::string City_map::get_info(Point p)
{
  return get_info(p.x, p.y);
}

std::string City_map::get_info(int x, int y)
{
  if (is_oob(x, y)) {
    std::stringstream ret;
    ret << "Out of Bounds [" << x << ":" << y << "]";
    return ret.str();
  }

  return tiles[x][y].get_info();
}

int City_map::get_farmability(Point p)
{
  return get_farmability(p.x, p.y);
}

int City_map::get_farmability(int x, int y)
{
  if (is_oob(x, y)) {
    return 0;
  }
  return tiles[x][y].get_farmability();
}

bool City_map::is_oob(int x, int y)
{
  return (x < 0 || x >= CITY_MAP_SIZE || y < 0 || y >= CITY_MAP_SIZE);
}

