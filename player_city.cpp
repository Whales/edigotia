#include "player_city.h"
#include "cuss.h"
#include "window.h"
#include "building.h"
#include "stringfunc.h"
#include "geometry.h"
#include "rng.h"
#include "combat.h" // For hunting
#include <sstream>
#include <vector>
#include <map>

Player_city::Player_city()
{
  type = CITY_TYPE_CITY;
  race = RACE_NULL;

  for (int i = 0; i < CIT_MAX; i++) {
    population[i].type = Citizen_type(i);
    tax_rate[i] = 0;
  }

  birth_points = 0;

  for (int i = 0; i < RES_MAX; i++) {
    resources[i] = 0;
  }
  for (int i = 0; i < MINERAL_MAX; i++) {
    minerals[i] = 0;
  }
  for (int i = 0; i < ANIMAL_MAX; i++) {
    hunting_action[i] = Animal_data[i]->default_action;
  }

  radius = 1;
  unread_messages = 0;
}

Player_city::~Player_city()
{
}

bool Player_city::place_keep()
{
  Window w_map(0, 0, 40, 24);
  cuss::interface i_map;
  if (!i_map.load_from_file("cuss/city_map.cuss")) {
    return false;
  }

  i_map.set_data("text_info", "Start town here?\n<c=magenta>Y/N<c=/>");

  do {
    draw_map(i_map.find_by_name("draw_map"));
    i_map.set_data("draw_map", glyph('@', c_yellow, c_black),
                   CITY_MAP_SIZE / 2, CITY_MAP_SIZE / 2);
    i_map.draw(&w_map);
    w_map.refresh();
    long ch = getch();
    if (ch == 'Y' || ch == 'y') {
      Area keep(AREA_KEEP, Point(CITY_MAP_SIZE / 2, CITY_MAP_SIZE / 2));;
      areas.push_back(keep);
      return true;
    } else if (ch == 'n' || ch == 'N') {
      return false;
    }
  } while (true);
  return false;
}

void Player_city::pick_race()
{
// A list of options.
  std::vector<std::string> options;
  for (int i = 1; i < RACE_MAX; i++) {
    options.push_back( Race_data[i]->plural_name );
  }
  int race_index = 1 + menu_vec("Pick race:", options);
  race = Race(race_index);
}

void Player_city::start_new_city()
{
  City::start_new_city();

  Race_datum* race_dat = Race_data[race];

  for (int i = 0; i < CIT_MAX; i++) {
    if (i >= CIT_PEASANT && i <= CIT_BURGHER) {
      set_tax_rate( Citizen_type(i), race_dat->high_tax_rate[i] - 5 );
    } else {
      tax_rate[i] = 0;
    }
  }

  birth_points = 0;

  radius = 1;
}

glyph Player_city::get_glyph()
{
  glyph ret;
  ret.bg = c_black;
  int pop = get_total_population();
  switch (type) {
    case CITY_TYPE_CAPITAL:
      ret.symbol = '@';
      break;
    case CITY_TYPE_DUCHY:
      ret.symbol = 'D';
      break;
    case CITY_TYPE_CITY:
    default:
      if (pop >= CITY_POPULATION_LARGE) {
        ret.symbol = 'O';
      } else {
        ret.symbol = 'o';
      }
      break;
  }

// Now color us based on our population.
  int pop_level = 0;
  if (pop >= CITY_POPULATION_LARGE) {
// 100 if we're barely large, 200 if we're double-large, etc
    pop_level = (100 * pop) / CITY_POPULATION_LARGE;
  } else {
// 100 if we're half-large, 200 if we're nearly large, 0 if we're 0
    pop_level = (200 * pop) / CITY_POPULATION_LARGE;
  }

  if (pop_level >= 200) {
    ret.fg = c_white;
  } else if (pop_level >= 175) {
    ret.fg = c_ltred;
  } else if (pop_level >= 150) {
    ret.fg = c_yellow;
  } else if (pop_level >= 125) {
    ret.fg = c_ltgreen;
  } else if (pop_level >= 100) {
    ret.fg = c_ltblue;
  } else if (pop_level >= 75) {
    ret.fg = c_cyan;
  } else if (pop_level >= 50) {
    ret.fg = c_magenta;
  } else if (pop_level >= 25) {
    ret.fg = c_brown;
  } else {
    ret.fg = c_ltgray;
  }

  return ret;
}

// radius_limited and only_terrain default to false.
void Player_city::draw_map(cuss::element* e_draw, Point sel, bool radius_limited,
                    bool only_terrain)
{
  if (!e_draw) {
    debugmsg("Player_city::draw_map() called with NULL drawing area.");
    return;
  }

  std::map<Point,glyph,Pointcomp> drawing;

// Draw any constructed areas
  if (!only_terrain) {
    for (int i = 0; i < areas.size(); i++) {
      Area* area = &(areas[i]);
      Area_datum* areadata = Area_data[area->type];
      glyph sym = areadata->symbol;
      if (area->pos == sel) {
        sym.bg = c_blue;
      } else if (!area->open) {
        sym.bg = c_ltgray;
      }
      drawing[area->pos] = sym;
    }

// Draw any enqueued areas
    for (int i = 0; i < area_queue.size(); i++) {
      Area* area = &(area_queue[i]);
      Area_datum* areadata = Area_data[area->type];
      glyph gl = areadata->symbol;
      if (area->pos == sel) {
        gl.bg = c_blue;
      } else {
        gl.bg = c_brown;
      }
      drawing[area->pos] = gl;
    }
  }

// Now for any "unclaimed" points, pull the glyph from our map
  for (int x = 0; x < CITY_MAP_SIZE; x++) {
    for (int y = 0; y < CITY_MAP_SIZE; y++) {
      Point pos(x, y);
      if (drawing.count(pos) == 0) {
        glyph gl = map.get_glyph(pos);
        if (radius_limited && !inside_radius(pos)) {
          gl.fg = c_dkgray;
        }
        if (pos == sel) {
          gl.bg = c_blue;
        }
        drawing[pos] = gl;
      }
    }
  }

// Finally, draw the glyphs to e_draw.
  for (int x = 0; x < CITY_MAP_SIZE; x++) {
    for (int y = 0; y < CITY_MAP_SIZE; y++) {
      Point pos(x, y);
      if (drawing.count(pos) == 0) {
        debugmsg("ERROR - hole in city drawing at %s!", pos.str().c_str());
        e_draw->set_data(glyph(), x, y);
      } else {
        e_draw->set_data(drawing[pos], x, y);
      }
    }
  }
}

void Player_city::do_turn()
{
// Birth a new citizen(s)?
  birth_points += get_daily_birth_points();
  int births = 0;
  while (birth_points >= 100) {
    birth_points -= 100;
    births++;
  }
  birth_citizens(births); // Handles the message, too

// Import resources.
  for (int i = 1; i < RES_MAX; i++) {
    Resource import = Resource(i);
    resources[import] += get_import(import);
  }

// Receive taxes.
  resources[RES_GOLD] += get_taxes();

// Check for starvation.
  for (int i = 1; i < CIT_MAX; i++) {
    int starve_chance = population[i].get_starvation_chance();
// TODO: Tweak this?
    if (starve_chance > 3) {
      starve_chance = (starve_chance * starve_chance) / 3;
    }
    int dead = 0;
/* Days: Odds:  (Example: after 5 days, each citizen has an 8% chance of dying).
     1     1%
     2     2%
     3     3%
     4     5%
     5     8%
    10    33%
    17    96%
*/
    if (starve_chance >= 100) {
      dead = population[i].count; // Game over, man!
    } else if (starve_chance > 0) {
      for (int n = 0; n < population[i].count; n++) {
        if (rng(1, 100) <= starve_chance) {
          dead++;
        }
      }
    }
// TODO: Alert the player to the death of citizens.
// TODO: Lose morale due to starvation?
    kill_citizens( Citizen_type(i), dead, DEATH_STARVATION );
  } // for (int i = 0; i < CIT_MAX; i++)

// Produce / eat food.
  resources[RES_FOOD] += get_food_production();
  int food_consumed = get_food_consumption();
  if (resources[RES_FOOD] >= food_consumed) {
    resources[RES_FOOD] -= food_consumed;
// Everyone eats!  Reduce starvation.
    for (int i = 0; i < CIT_MAX; i++) {
      if (population[i].starvation <= 5) {
        population[i].starvation = 0;
      } else {
        population[i].starvation /= 2;
      }
    }
  } else {
// We can't feed everyone!  So figure out who we can feed.
// The upper classes get to eat first.
// TODO: Allow a law which changes this?
    for (int i = CIT_MAX - 1; i > CIT_NULL; i--) {
      Citizen_type cit_type = Citizen_type(i);
      int type_consumption = get_food_consumption(cit_type);
      int* ptr_starvation = &(population[i].starvation);
      if (resources[RES_FOOD] >= type_consumption)  {
        resources[RES_FOOD] -= type_consumption;
// We ate, hooray!  Reduce starvation.
        if (*ptr_starvation <= 5) {
          *ptr_starvation = 0;
        } else {
          *ptr_starvation /= 2;
        }
      } else {
        int food_deficit = type_consumption - resources[RES_FOOD];
        int citizen_consumption = citizen_food_consumption(cit_type);
        int hungry_citizens = 0;
        if (citizen_consumption > 0) {
          hungry_citizens = food_deficit / citizen_consumption;
        }
// Everyone starves.  No reduction in starvation, only an increase.
        if (hungry_citizens >= population[cit_type].count) {
          *ptr_starvation += hungry_citizens;

        } else { // Some people eat; normal increase, then a reduction
          int pop = population[cit_type].count;
          *ptr_starvation += hungry_citizens;
/* At best (0 hungry citizens), this is equivalent to dividing starvation by 2;
 * which is exactly what we do when all citizens are fed.  As hungry_citizens
 * approaches pop this gets closer to dividing starvation by 1; which is exactly
 * what we do when no citizens are fed.
 */
          *ptr_starvation = (*ptr_starvation * (pop + hungry_citizens)) /
                            (pop * 2);
        }
        add_message(MESSAGE_URGENT, "We have run out of food!");
        resources[RES_FOOD] = 0;
      }
    }
  }

// Handle building production.
  for (int i = 0; i < buildings.size(); i++) {
    Building* bldg = &(buildings[i]);
    if (!bldg->build_queue.empty()) {
      Recipe_amount* rec_amt = &(bldg->build_queue[0]);
      Recipe* rec = &(rec_amt->recipe);
// Check if we can build the recipe today.
      int num_built = 0;
      if (rec->days_per_unit <= 1) {  // Can build it every day!
        num_built = bldg->workers;
        if (rec->units_per_day > 0) {
          num_built *= rec->units_per_day;
        }
      } else {
// Each worker reduces our counter by 1.
        rec_amt->days_until_built -= bldg->workers;
/* We might be able to build more than 1 per day.  If days_per_unit is 2, and we
 * have 5 workers, we'll produce 3 units the first day and wind up with
 * days_until_built of 1.  The next day we'll produce 2 untils and wind up with
 * days_until_built of 0.
 */
        while (rec_amt->days_until_built < 0) {
          num_built++;
          rec_amt->days_until_built += rec->days_per_unit;
        }
      } // rec->days_per_unit > 1
// Make sure we won't build more than are enqueued.
      if (rec_amt->amount != INFINITE_RESOURCE && num_built > rec_amt->amount) {
        num_built = rec_amt->amount;
      }
// Perform the following once for each unit built.
      for (int n = 0; n < num_built; n++) {
// Check that we have the resources and minerals before expending them, so that
// we don't expend one only to find we lack the other.
        if (has_resources(rec->resource_ingredients) &&
            has_minerals(rec->mineral_ingredients)) {
          expend_resources(rec->resource_ingredients);
          expend_minerals (rec->mineral_ingredients );
          gain_resource   (rec->result);
          if (rec_amt->amount != INFINITE_RESOURCE) {
            rec_amt->amount--;
          }
        }
      }
// Check if we've built all that we enqueued.
      if (rec_amt->amount != INFINITE_RESOURCE && rec_amt->amount <= 0) {
        bldg->build_queue.erase( bldg->build_queue.begin() );
      }
    } // if (!bldg->build_queue.empty())
  } // for (int i = 0; i < buildings.size(); i++)

// Produce minerals from mines, wood from sawmills, and hunts from hunting camps
  for (int i = 0; i < areas.size(); i++) {

    if (areas[i].produces_resource(RES_MINING)) { // It's a mine!
      Building* mine_building = &(areas[i].building);
      Point mine_pos = areas[i].pos;
      Map_tile* tile = map.get_tile(mine_pos);

      for (int n = 0; n < mine_building->minerals_mined.size(); n++) {
        Mineral_amount min_mined = mine_building->minerals_mined[n];

        if (min_mined.amount > 0) {
          int workers = min_mined.amount; // In case we need to fire them; below
          min_mined.amount *= mine_building->shaft_output;
// Check that the terrain still has enough of that resource!
          bool found_mineral = false;

          for (int m = 0; !found_mineral && m < tile->minerals.size(); m++) {

            if (tile->minerals[m].type == min_mined.type) {
              found_mineral = true;
              Mineral_amount* tile_min = &(tile->minerals[m]);

              if (tile_min->amount == INFINITE_RESOURCE) {
                minerals[min_mined.type] += min_mined.amount;

              } else if (tile_min->amount < min_mined.amount) {
                Mineral_datum* min_dat = Mineral_data[min_mined.type];
                add_message(MESSAGE_MAJOR,
                            "Our mine has exhausted its supply of %s!",
                            min_dat->name.c_str());

                minerals[min_mined.type] += tile_min->amount;
                tile->minerals.erase( tile->minerals.begin() + m );
// Fire any workers associated with that mineral.
// TODO: Don't hardcode CIT_PEASANT, even though it's a perfectly OK assumption
                fire_citizens(CIT_PEASANT, workers, mine_building);
                mine_building->minerals_mined.erase(
                  mine_building->minerals_mined.begin() + n
                );
                n--;

              } else { // Not infinite, nor have we run out of the mineral
                minerals[min_mined.type] += min_mined.amount;
                tile_min->amount -= min_mined.amount;
              }
            } // if (tile->minerals[m].type == min_mined.type)
          } // for (int m = 0; !found_mineral && m < tile->minerals.size(); m++)
        } // if (min_mined.amount > 0)
      } // for (int n = 0; n < mine_building->minerals_mined.size(); n++)
    } // if (areas[i].produces_resource(RES_MINING))

    if (areas[i].produces_resource(RES_LOGGING)) { // Sawmill etc
      Building* sawmill_bldg = &(areas[i].building);
      Point mine_pos = areas[i].pos;
      Map_tile* tile = map.get_tile(mine_pos);
      int wood_produced = (sawmill_bldg->workers *
                           sawmill_bldg->amount_produced(RES_LOGGING));
// Alter logging based on racial ability.
// Skill level of 5 = no reduction, 1 = 1/5th of the normal rate.
      wood_produced = (wood_produced *
                       Race_data[race]->skill_level[SKILL_FORESTRY]) / 5;

      if (tile->wood != INFINITE_RESOURCE && tile->wood < wood_produced) {
        add_message(MESSAGE_MAJOR,
                    "Our %s has cleared the %s and is now closed.",
                    areas[i].get_name().c_str(),
                    tile->get_terrain_name().c_str());

        resources[RES_WOOD] += tile->wood;
        tile->wood = 0;
        tile->clear_wood();
        areas[i].close(this);

      } else { // We have not run out of wood.
        resources[RES_WOOD] += wood_produced;
        if (tile->wood != INFINITE_RESOURCE) {
          tile->wood -= wood_produced;
        }
      }

    } // if (areas[i].produces_resource(RES_LOGGING))

    if (areas[i].produces_resource(RES_HUNTING)) {  // It's a hunting camp!
      do_hunt( &(areas[i]) );
    }

  } // for (int i = 0; i < areas.size(); i++)

// Pay wages.
  int wages = get_total_wages();
  if (!expend_resource(RES_GOLD, wages)) {
// TODO: Consequences for failure to pay wages!
    resources[RES_GOLD] = 0;
    minerals[MINERAL_GOLD] = 0;
  }

// Lose gold to corruption.
  int corruption = get_corruption_amount();
  if (!expend_resource(RES_GOLD, corruption)) {
    resources[RES_GOLD] = 0;
    minerals[MINERAL_GOLD] = 0;
// TODO: Consequences for failure to pay corruption?
  }

// We total maintenance into a single pool because we'll need to divide the gold
// by 10, and we want to lose as little to rounding as possible.
  std::map<Resource,int> total_maintenance;
// Deduct maintenance for all areas
  for (int i = 0; i < areas.size(); i++) {
    if (areas[i].open) {
      std::map<Resource,int> maintenance = areas[i].get_maintenance();
      for (std::map<Resource,int>::iterator it = maintenance.begin();
           it != maintenance.end();
           it++) {
        if (total_maintenance.count(it->first)) {
          total_maintenance[it->first] += it->second;
        } else {
          total_maintenance[it->first] = it->second;
        }
      }
    }
  }
// Deduct maintenance for all buildings
  for (int i = 0; i < buildings.size(); i++) {
    std::map<Resource,int> maintenance = buildings[i].get_maintenance();
    for (std::map<Resource,int>::iterator it = maintenance.begin();
         it != maintenance.end();
         it++) {
      if (total_maintenance.count(it->first)) {
        total_maintenance[it->first] += it->second;
      } else {
        total_maintenance[it->first] = it->second;
      }
    }
  }
  if (total_maintenance.count(RES_GOLD)) {
    total_maintenance[RES_GOLD] /= 10;
  }
  if (!expend_resources(total_maintenance)) {
// TODO: Close some areas until we CAN pay this.
  }

// The last resource transaction we should do is exporting resources, since it's
// presumably the one with the least consequences.
  for (int i = 1; i < RES_MAX; i++) {
    Resource res_export = Resource(i);
    int export_amt = get_export(res_export);
    if (resources[res_export] >= export_amt) {
      resources[res_export] += export_amt;
    } else {
// TODO: consequences for failure to export
    }
  }

// Make sure our food isn't above the cap (food goes bad y'know)
  int food_cap = get_food_cap();
  if (resources[RES_FOOD] > food_cap) {
    resources[RES_FOOD] = food_cap;
  }

// Allow mines to discover new materials.
// TODO: Make this better (put in its own function?)
  for (int i = 0; i < areas.size(); i++) {
    Building* area_build = &(areas[i].building);
    for (int n = 0; n < area_build->minerals_mined.size(); n++) {
      Mineral_amount* min_amt = &(area_build->minerals_mined[n]);
      Map_tile* tile_here = map.get_tile( areas[i].pos );
      int amount_buried = 0;
      if (tile_here) {
        amount_buried = tile_here->get_mineral_amount(min_amt->type);
      }
      if (min_amt->amount == HIDDEN_RESOURCE &&
          (amount_buried == INFINITE_RESOURCE ||
           rng(1, 20000) < amount_buried)) {
        add_message(MESSAGE_MAJOR, "Our mine has discovered %s!",
                    Mineral_data[min_amt->type]->name.c_str());
        min_amt->amount = 0;
      }
    }
  }

// Advance progress on the first area in our queue.
  if (!area_queue.empty()) {
    Area* area_to_build = &(area_queue[0]);
    area_to_build->building.construction_left--;
    if (area_to_build->building.construction_left <= 0) {
      add_message(MESSAGE_MINOR, "Our %s has finished construction.",
                  area_queue[0].get_name().c_str());
      add_open_area(area_queue[0]);
      area_queue.erase( area_queue.begin() );
    }
  }

// Advance progress on the first building in our queue.
  if (!building_queue.empty()) {
    Building* building_to_build = &(building_queue[0]);
    building_to_build->construction_left--;
    if (building_to_build->construction_left <= 0){
      add_message(MESSAGE_MINOR, "Our %s has finished construction.",
                  building_queue[0].get_name().c_str());
      add_open_building(building_queue[0]);
      building_queue.erase( building_queue.begin() );
    }
  }
}

Area_queue_status Player_city::add_area_to_queue(Area_type type, Point location)
{
  if (!inside_radius(location)) {
    return AREA_QUEUE_OUTSIDE_RADIUS;
  }

  Terrain_datum* ter_dat = map.get_terrain_datum(location);
  bool build_ok = false;
  for (int i = 0; !build_ok && i < ter_dat->buildable_areas.size(); i++) {
    if (type == ter_dat->buildable_areas[i]) {
      build_ok = true;
    }
  }

  if (!build_ok) {
    return AREA_QUEUE_BAD_TERRAIN;
  }

  if (area_at(location)) {
    return AREA_QUEUE_OCCUPIED;
  }

  Area tmp(type, location);
  return add_area_to_queue(tmp);
}

Area_queue_status Player_city::add_area_to_queue(Area area)
{
  area.make_queued();  // Sets up construction_left.
  area_queue.push_back(area);
  return AREA_QUEUE_OK;
}

void Player_city::add_open_area(Area area)
{
// Set it as open
  area.open = true;
// Figure out how many crops per field we get
  Building_datum* build_dat = area.get_building_datum();
  if (!build_dat) {
    debugmsg("NULL Building_data* in Player_city::open_area (%s).",
             area.get_name().c_str());
  }

// Farms are set up specially.
// Check if this area produces RES_FARMING.
  int farming = 0;
  for (int i = 0; farming == 0 && i < build_dat->production.size(); i++) {
    if (build_dat->production[i].type == RES_FARMING) {
      farming = build_dat->production[i].amount; // The amount of RES_FARMING
    }
  }
  if (farming > 0) {
    Map_tile* tile_here = map.get_tile(area.pos);
    Building* farm_bld = &(area.building);
    farming = (farming * tile_here->get_farmability()) / 100;
// Alter farming based on racial ability.
// Skill level of 5 = no reduction, 1 = 1/5th of the normal rate.
    farming = (farming * Race_data[race]->skill_level[SKILL_FARMING]) / 5;
    farm_bld->field_output = farming;
// Set up area.building's list of crops based on what's available here.
    farm_bld->crops_grown.clear();
    std::vector<Crop> crops_here = map.get_tile(area.pos)->crops;
    for (int i = 0; i < crops_here.size(); i++) {
      farm_bld->crops_grown.push_back( Crop_amount( crops_here[i], 0 ) );
    }
  }

// Mines are set up specially.
// Check if this area produces RES_MINING.
  int mining = 0;
  for (int i = 0; mining == 0 && i < build_dat->production.size(); i++) {
    if (build_dat->production[i].type == RES_MINING) {
      mining = build_dat->production[i].amount;
    }
  }
  if (mining > 0) {
// Alter mining based on racial ability.
// Skill level of 5 = no reduction, 1 = 1/5th of the normal rate.
    mining = (mining * Race_data[race]->skill_level[SKILL_MINING]) / 5;
// Set up area.building's list of minerals based on what's available here.
    area.building.shaft_output = mining;
    area.building.minerals_mined.clear();
    std::vector<Mineral_amount> mins_here = map.get_tile(area.pos)->minerals;
    for (int i = 0; i < mins_here.size(); i++) {
      Mineral mineral = mins_here[i].type;
/* An amount of HIDDEN_RESOURCE means that this mineral will be hidden (gasp)
 * from the player until it's discovered by random chance (or spells etc).  At
 * that point the amount will be changed to 0, and the player can increase it
 * further to indicate that they wish to mine more.
 * Note that almost all minerals are hidden (stone is not).
 */
      int mineral_amount = 0;
      if (Mineral_data[mineral]->hidden) {
        mineral_amount = HIDDEN_RESOURCE;
      }
      Mineral_amount tmp_amount( mineral, mineral_amount );
      area.building.minerals_mined.push_back(tmp_amount);
    }
  }

// Hunting camps are set up specially.
  if (area.produces_resource(RES_HUNTING)) {
    int level = Race_data[race]->base_combat / 2 +
                3 * Race_data[race]->skill_level[SKILL_HUNTING];
    area.building.hunter_level = level;
  }

// Now attempt to employ citizens to fill it up.
  Building* area_bldg = &(area.building);
  int num_jobs = area_bldg->get_total_jobs();
  Citizen_type cit_type = area_bldg->get_job_citizen_type();
  if (employ_citizens(cit_type, num_jobs, area_bldg)) {
    add_message(MESSAGE_MINOR, "%d %s have started work at the %s.",
                num_jobs, citizen_type_name(cit_type, true).c_str(),
                area.get_name().c_str());
// If it's a farm, we need to set crops to grow.
    if (area_bldg->produces_resource(RES_FARMING)) {
// Find whatever crop produces the most food.
      int best_index = -1, best_food = -1;
      for (int i = 0; i < area_bldg->crops_grown.size(); i++) {
        Crop crop = area_bldg->crops_grown[i].type;
        int food = Crop_data[crop]->food;
        if (food > best_food) {
          best_index = i;
          best_food = food;
        }
      }
      if (best_index >= 0) {
        Crop crop = area_bldg->crops_grown[best_index].type;
        Crop_datum* crop_dat = Crop_data[crop];
        add_message(MESSAGE_MINOR, "Our %s is now growing %s.",
                    area.get_name().c_str(), crop_dat->name.c_str());
        area_bldg->crops_grown[best_index].amount += num_jobs;
      } else {
        add_message(MESSAGE_MAJOR, "Our %s needs to select a crop to grow.",
                    area.get_name().c_str());
      }
    }
// Mines need to have minerals chosen
    if (area_bldg->produces_resource(RES_MINING)) {
// Find whatever mineral is worth the most
      int best_index = -1, best_value = -1;
      for (int i = 0; i < area_bldg->minerals_mined.size(); i++) {
        Mineral mineral = area_bldg->minerals_mined[i].type;
        int value = Mineral_data[mineral]->value;
        if (area_bldg->minerals_mined[i].amount != HIDDEN_RESOURCE && 
            value > best_value) {
          best_index = i;
          best_value = value;
        }
      }
      if (best_index >= 0) {
        Mineral min = area_bldg->minerals_mined[best_index].type;
        Mineral_datum* min_dat = Mineral_data[min];
        add_message(MESSAGE_MINOR, "Our %s is now mining %s.",
                    area.get_name().c_str(), min_dat->name.c_str());
        area_bldg->minerals_mined[best_index].amount += num_jobs;
      } else {
        add_message(MESSAGE_MAJOR, "Our %s needs to select a mineral to mine.",
                    area.get_name().c_str());
      }
    }
  } else if (num_jobs > 0) {
    add_message(MESSAGE_MINOR, "Our %s could not hire citizens.",
                area.get_name().c_str());
  }

  areas.push_back( area );

}

void Player_city::destroy_area_at(int x, int y)
{
  destroy_area_at( Point(x, y) );
}

void Player_city::destroy_area_at(Point pos)
{
  for (int i = 0; i < areas.size(); i++) {
    if (areas[i].pos == pos) {
      int cost = areas[i].get_building_datum()->destroy_cost;
      if (expend_resource(RES_GOLD, cost)) {
        areas[i].close(this); // Should handle everything needed to destroy it
        areas.erase( areas.begin() + i );
      }
      return;
    }
  }

// Check under-construction areas too
  for (int i = 0; i < area_queue.size(); i++) {
    if (area_queue[i].pos == pos) {
      area_queue.erase( area_queue.begin() + i );
      return;
    }
  }
}

Building_queue_status Player_city::add_building_to_queue(Building_type type)
{
  Building tmp;
  tmp.set_type(type);
  return add_building_to_queue(tmp);
}

Building_queue_status Player_city::add_building_to_queue(Building building)
{
  Building_datum* bldg_dat = building.get_building_datum();
  if (!expend_resources(bldg_dat->build_costs)) {
    return BUILDING_QUEUE_NO_RESOURCES;
  }

  building.make_queued();
  building_queue.push_back(building);
  return BUILDING_QUEUE_OK;
}

void Player_city::add_open_building(Building building)
{
// TODO: Anything at all?  Should we try to auto-employ workers?
  buildings.push_back(building);
}

bool Player_city::cancel_queued_building(int index)
{
  if (index < 0 || index >= building_queue.size()) {
    return false;
  }

// We get our resources back!
  Building_datum* bldg_dat = building_queue[index].get_building_datum();

  gain_resources(bldg_dat->build_costs);

  return true;
}

bool Player_city::employ_citizens(Citizen_type type, int amount, Building* job_site)
{
// Several checks to ensure we can make this assignment
  if (!job_site) {
    return false;
  }
  if (type == CIT_NULL) {
    return false; // Gotta be a real class.
  }
  if (amount <= 0) {
    return false;
  }
  if (population[type].get_unemployed() < amount) {
    return false;
  }
  if (job_site->get_available_jobs(type) < amount) {
    return false;
  }

// Okay!  We can do it!
  population[type].employed += amount;
  job_site->workers += amount;
  return true;
}

bool Player_city::fire_citizens(Citizen_type type, int amount,
                                Building* job_site)
{
// Several checks to ensure we can make this assignment
  if (!job_site) {
    return false;
  }
  if (type == CIT_NULL) {
    return false; // Gotta be a real class.
  }
  if (amount <= 0) {
    return false;
  }
  if (population[type].count < amount) {
    return false;
  }
  if (job_site->get_filled_jobs(type) < amount) {
    return false;
  }

// Okay!  We can do it!
  population[type].employed -= amount;
  job_site->workers -= amount;
  return true;
}

void Player_city::kill_citizens(Citizen_type type, int amount,
                                Cause_of_death cause, std::string cause_text)
{
// Sanity check
  if (type == CIT_NULL || type == CIT_MAX) {
    debugmsg("Player_city::kill_citizens(%s) called!",
             citizen_type_name(type).c_str());
    return;
  }

  if (amount <= 0) {
    return;
  }

  if (population[type].count < amount) {
    amount = population[type].count;
  }

// Figure out if we killed anyone who's employed.  If so, we need to fire them.
// We kill off the unemployed first, so to get the number dead who are employed,
// we subtrace the unemployed from the amount killed.
  int employed_killed = amount - get_unemployed_citizens(type);
// Randomly pick buildings to remove employees from
  std::vector<Building*> employers = get_employers(type);
  while (employed_killed > 0 && !employers.empty()) {
    int index = rng(0, employers.size());
    Building* bldg = employers[index];
    if (bldg->workers <= 0) {
      debugmsg("Player_city::killed_citizens() tried to fire citizens from a \
workerless building!");
      employers.erase( employers.begin() + index );
    } else {
      fire_citizens(type, 1, bldg);
      if (bldg->workers <= 0) {
        employers.erase( employers.begin() + index );
      }
    }
  }

// OK, now we can kill them off.
  population[type].remove_citizens(amount);

// And add a message.
  if (cause_text.empty()) {
    cause_text = cause_of_death_text(cause, (amount > 1));
  }

  add_message(MESSAGE_URGENT, "%d %s %s.",
              amount,
              citizen_type_name(type, (amount > 1)).c_str(),
              cause_text.c_str());
// TODO: Morale penalty here.
// TODO: Do something with our Cause_of_death.  Modify the message for sure, but
//       more?  Stats?
}

void Player_city::add_message(Message_type type, std::string text, ...)
{
  if (type == MESSAGE_NULL || type == MESSAGE_MAX) {
    debugmsg("Player_city::add_message() called with type %d.", type);
    return;
  }

  if (text.empty()) {
    return; // Might happen for valid reasons, so no debugmsg()
  }

  va_list ap;
  va_start(ap, text);
  char buff[8192];
  vsprintf(buff, text.c_str(), ap);
  va_end(ap);

  std::string formatted_text = buff;

  Message new_message(type, formatted_text);
  if (game) {
    new_message.date = game->get_date();
  }

  unread_messages++;
  messages.push_back(new_message);
}

std::vector<int> Player_city::get_unread_message_count()
{
  std::vector<int> ret;
  for (int i = 0; i < MESSAGE_MAX; i++) {
    ret.push_back(0);
  }

  if (unread_messages == 0) {
    return ret;
  }

  for (int i = messages.size() - unread_messages; i < messages.size(); i++) {
    ret[ messages[i].type ]++;
  }

  return ret;
}

bool Player_city::inside_radius(int x, int y)
{
  return inside_radius( Point(x, y) );
}

bool Player_city::inside_radius(Point p)
{
  Point center(CITY_MAP_SIZE / 2, CITY_MAP_SIZE / 2);

  return rl_dist(center, p) <= radius;
}

Area* Player_city::area_at(int x, int y)
{
  return area_at( Point(x, y) );
}

Area* Player_city::area_at(Point p)
{
  for (int i = 0; i < areas.size(); i++) {
    if (areas[i].pos == p) {
      return &(areas[i]);
    }
  }
  for (int i = 0; i < area_queue.size(); i++) {
    if (area_queue[i].pos == p) {
      return &(area_queue[i]);
    }
  }
  return NULL;
}

std::string Player_city::get_map_info(Point p)
{
  std::stringstream ret;
  ret << map.get_info(p);
  Area* area = area_at(p);
  if (area) {
    ret << std::endl;
    ret << "<c=pink>" << area->get_name() << "<c=/>";
    if (area->building.construction_left > 0) {
      ret << " (<c=brown>Under Construction<c=/>)";
    } else if (!area->open) {
      ret << " (<c=red>Closed<c=/>)";
    }
  }

  return ret.str();
}

// type defaults to CIT_NULL
int Player_city::get_total_population(Citizen_type type)
{
  if (type == CIT_NULL) {
    int ret = 0;
    for (int i = 0; i < CIT_MAX; i++) {
      ret += population[i].count;
    }
    return ret;
  }
  return population[type].count;
}

// type defaults to CIT_NULL
int Player_city::get_unemployed_citizens(Citizen_type type)
{
  if (type == CIT_NULL) {
    int ret = 0;
    for (int i = 0; i < CIT_MAX; i++) {
      ret += population[i].get_unemployed();
    }
    return ret;
  }
  return population[type].get_unemployed();
}

// type defaults to CIT_NULL
int Player_city::get_total_housing(Citizen_type type)
{
  int ret = 0;
  for (int i = 0; i < areas.size(); i++) {
    Building_datum* build_dat = areas[i].get_building_datum();
    if (build_dat) {
      for (int n = 0; n < build_dat->housing.size(); n++) {
        if (type == CIT_NULL || type == build_dat->housing[n].type) {
          ret += build_dat->housing[n].amount;
        }
      }
    }
  }
  for (int i = 0; i < buildings.size(); i++) {
    Building_datum* build_dat = buildings[i].get_building_datum();
    if (build_dat) {
      for (int n = 0; n < build_dat->housing.size(); n++) {
        if (type == CIT_NULL || type == build_dat->housing[n].type) {
          ret += build_dat->housing[n].amount;
        }
      }
    }
  }

  return ret;
}

int Player_city::get_military_count()
{
  int ret = 0;
  for (int i = 0; i < units_stationed.size(); i++) {
    ret += units_stationed[i].count;
  }
  return ret;
}

int Player_city::get_military_supported()
{
  int ret = 0;
  for (int i = 0; i < areas.size(); i++) {
    Building_datum* build_dat = areas[i].get_building_datum();
    if (build_dat) {
      ret += build_dat->military_support;
    }
  }
  for (int i = 0; i < buildings.size(); i++) {
    Building_datum* build_dat = buildings[i].get_building_datum();
    if (build_dat) {
      ret += build_dat->military_support;
    }
  }

  return ret;
}

int Player_city::get_daily_birth_points()
{
  int ret = 0;
  int pop[CIT_MAX], rate[CIT_MAX];
  for (int i = CIT_NULL; i < CIT_MAX; i++) {
    pop[i] = population[i].count;
    rate[CIT_MAX] = 0;
  }

// Specific rates for each type.  Lower values mean a faster rate.
  for (int i = CIT_PEASANT; i <= CIT_BURGHER; i++) {
    rate[i] = Race_data[race]->birth_rate[i];
  }
  
  for (int i = CIT_PEASANT; i < CIT_MAX; i++) {
    if (pop[i] > 0) {
      ret += pop[i] / rate[i];
// Chance to let partial population increase this by 1.
      if (rng(1, rate[i]) <= pop[i] % rate[i]) {
        ret++;
      }
    }
  }

// TODO: Other modifiers?  Food, health, morale, etc
  return ret;
}

int Player_city::get_required_ratio(Citizen_type cit_type)
{
  return Race_data[race]->citizen_ratio[cit_type];
}

int Player_city::get_population_cap(Citizen_type cit_type)
{
  if (cit_type <= CIT_PEASANT) {
    return 0;
  }
  int ratio = get_required_ratio(cit_type);
  int type_below = cit_type - 1;
  return population[type_below].count / ratio;
}

int Player_city::get_chance_to_birth(Citizen_type cit_type)
{
  if (cit_type == CIT_NULL || cit_type == CIT_SLAVE || cit_type == CIT_MAX) {
    return 0; // These are never birthed.
  }

  Citizen_type lower_class = Citizen_type(cit_type - 1);

// Ensure we have enough citizens of the class below this one to support another
// citizen of this class.
  int req = (population[cit_type].count + 1) * get_required_ratio(cit_type);
  if (population[lower_class].count < req) {
    return 0; // We don't meet the required ratio!
  }

// Ensure we have sufficient housing
  if (get_total_housing(cit_type) <= population[cit_type].count) {
    return 0;
  }

// TODO: Check the morale of the class below this one, among other factors.
  return 100;
}

// num defaults to 1
void Player_city::birth_citizens(int num)
{
  if (num <= 0) {
    return;
  }
// Total born, for our message below
  std::vector<Citizen_amount> born;
  for (int i = 0; i < num; i++) {
// Decide what type the new citizen will be.
    Citizen_type new_cit_type = CIT_NULL;
    for (int i = CIT_MAX - 1;
         new_cit_type == CIT_NULL && i >= CIT_PEASANT;
         i--) {
      Citizen_type cit_type = Citizen_type(i);
      int chance = get_chance_to_birth(cit_type);
      if (chance > 0 && rng(1, 100) <= chance) {
        new_cit_type = cit_type;
      }
    }
// Add to born, either an existing element or a new one
    bool found = false;
    for (int i = 0; !found && i < born.size(); i++) {
      if (born[i].type == new_cit_type) {
        found = true;
        born[i].amount++;
      }
    }
    if (!found) {
      born.push_back( Citizen_amount( new_cit_type, 1 ) );
    }
// Add the baby!
    population[new_cit_type].add_citizens(1);
  }

  if (born.empty()) { // Should never happen... right?
    return;
  }

// Draft and add a new message.
  std::stringstream ss_message;
  for (int i = 0; i < born.size(); i++) {

    if (i > 0) {  // Conjunction (or comma)
      if (i == born.size() - 1) {
        ss_message << " and ";
      } else {
        ss_message << ", ";
      }
    }

    if (born[i].amount == 1) { // Article (or the number)
      ss_message << "a";
    } else {
      ss_message << born[i].amount;
    }

    ss_message << " " << citizen_type_name( born[i].type );
  }
// Verb
  if (born.size() > 1 || born[0].amount > 1) {
    ss_message << " were born!";
  } else {
    ss_message << " was born!";
  }

  std::string message = capitalize( ss_message.str() );
  add_message( MESSAGE_MINOR, message );
}


std::vector<Building*> Player_city::get_all_buildings()
{
  std::vector<Building*> ret;
// First, open areas.
  for (int i = 0; i < areas.size(); i++) {
    ret.push_back( &(areas[i].building) );
  }
// Then, actual buildings.
  for (int i = 0; i < buildings.size(); i++) {
    ret.push_back( &(buildings[i]) );
  }

  return ret;
}

std::vector<Building*> Player_city::get_pure_buildings()
{
  std::vector<Building*> ret;
  for (int i = 0; i < buildings.size(); i++) {
    ret.push_back( &(buildings[i]) );
  }
  return ret;
}

// cit_type defaults to CIT_NULL
std::vector<Building*> Player_city::get_employers(Citizen_type cit_type)
{
  std::vector<Building*> ret;
  for (int i = 0; i < buildings.size(); i++) {
    Building* bldg = &(buildings[i]);
    if (bldg->get_filled_jobs(cit_type) > 0) {
      ret.push_back(bldg);
    }
  }
  return ret;
}

int Player_city::get_number_of_buildings(Building_type type)
{
  int ret = 0;
  for (int i = 0; i < buildings.size(); i++) {
    if (type == BUILD_NULL || buildings[i].type == type) {
      ret++;
    }
  }
  for (int i = 0; i < areas.size(); i++) {
    Building_type area_build = areas[i].get_building_type();
    if (area_build != BUILD_NULL &&
        (type == BUILD_NULL || area_build == type)) {
      ret++;
    }
  }
  return ret;
}

int Player_city::get_number_of_areas(Area_type type)
{
  int ret = 0;
  for (int i = 0; i < areas.size(); i++) {
    if (type == AREA_NULL || areas[i].type == type) {
      ret++;
    }
  }
  return ret;
}

int Player_city::get_total_maintenance()
{
  int ret = 0;
  for (int i = 0; i < areas.size(); i++) {
    if (areas[i].open) {
      ret += areas[i].get_building_datum()->upkeep;
    }
  }
  for (int i = 0; i < buildings.size(); i++) {
    ret += buildings[i].get_building_datum()->upkeep;
  }
  return ret / 10;  // Since maintenance is in 1/10th of a gold
}

int Player_city::get_fields_worked()
{
  int ret = 0;
  for (int i = 0; i < areas.size(); i++) {
    if (areas[i].open && areas[i].type == AREA_FARM) {
      ret += areas[i].building.workers;
    }
  }
  return ret;
}

int Player_city::get_empty_fields()
{
  int ret = 0;
  for (int i = 0; i < areas.size(); i++) {
    if (areas[i].open && areas[i].type == AREA_FARM) {
      Building_datum* build_dat = areas[i].get_building_datum();
      if (build_dat && areas[i].building.workers < build_dat->jobs.amount) {
        ret += build_dat->jobs.amount - areas[i].building.workers;
      }
    }
  }
  return ret;
}

int Player_city::get_shafts_worked()
{
  int ret = 0;
  for (int i = 0; i < areas.size(); i++) {
    if (areas[i].open && areas[i].type == AREA_MINE) {
      ret += areas[i].building.workers;
    }
  }
  return ret;
}

int Player_city::get_free_shafts()
{
  int ret = 0;
  for (int i = 0; i < areas.size(); i++) {
    if (areas[i].open && areas[i].type == AREA_MINE) {
      Building_datum* build_dat = areas[i].get_building_datum();
      if (build_dat && areas[i].building.workers < build_dat->jobs.amount) {
        ret += build_dat->jobs.amount - areas[i].building.workers;
      }
    }
  }
  return ret;
}

int Player_city::get_resource_production(Resource res)
{
// Check all buildings (including areas)
  std::vector<Building*> bldgs = get_all_buildings();
  int ret = 0;

  for (int i = 0; i < bldgs.size(); i++) {
    ret += bldgs[i]->amount_produced(res);
    ret += bldgs[i]->amount_built(res, this);
  }

  return ret;
}

// type defaults to CIT_NULL
int Player_city::get_total_wages(Citizen_type type)
{
  int ret = 0;
  for (int i = 0; i < areas.size(); i++) {
    Building_datum* build_dat = areas[i].get_building_datum();
    int workers = areas[i].building.workers;
    if (workers > 0 && (type == CIT_NULL || type == build_dat->jobs.type)) {
      ret += build_dat->wages * workers;
    }
  }
  for (int i = 0; i < buildings.size(); i++) {
    Building_datum* build_dat = buildings[i].get_building_datum();
    int workers = buildings[i].workers;
    if (workers > 0 && (type == CIT_NULL || type == build_dat->jobs.type)) {
      ret += build_dat->wages * workers;
    }
  }
// Wages are reported in 1/10th of a gold, so we need to divide by 10!
  return ret / 10;
}

// TODO: This function, for real
int Player_city::get_military_expense()
{
  int ret = 0;
  for (int i = 0; i < units_stationed.size(); i++) {
    ret += units_stationed[i].count / 5;
  }
  return ret;
}

void Player_city::set_tax_rate(Citizen_type type, int rate)
{
// Sanity check
  if (rate < 0 || rate > 100) {
    return;
  }
  tax_rate[type] = rate;
// TODO: Set these values elsewhere; maybe from race?
// Taxes below low_rate give a morale bonus.  Taxes above high_rate give a huge
// morale penalty.
  int low_rate = get_low_tax_rate(type), high_rate = get_high_tax_rate(type);
// Set tax_morale of the relevant population;
  if (rate < low_rate) {
    population[type].tax_morale = low_rate - rate;
  } else if (rate > high_rate) {
    population[type].tax_morale = -150;
  } else {
    population[type].tax_morale = 0 - (50 * (rate - low_rate)) / high_rate;
  }
}
// type defaults to CIT_NULL

int Player_city::get_low_tax_rate(Citizen_type type)
{
  return Race_data[race]->low_tax_rate[type];
}

int Player_city::get_high_tax_rate(Citizen_type type)
{
  return Race_data[race]->high_tax_rate[type];
}

int Player_city::get_taxes(Citizen_type type)
{
  if (type == CIT_NULL) {
    int ret = 0;
    for (int i = 1; i < CIT_MAX; i++) {
      ret += get_taxes( Citizen_type(i) );
    }
    return ret;
  }

  return (population[type].get_income() * tax_rate[type]) / 100;
}

// TODO: This function
int Player_city::get_corruption_percentage()
{
  return 10;
}

int Player_city::get_corruption_amount()
{
  int percentage = get_corruption_percentage();
// TODO: This is probably not an accurate total for income.  Maybe it should
//       have its own function?
  int income = get_taxes() + get_import(RES_GOLD);
  int lost = income * percentage;
  lost /= 100;  // Since percentage is reported as an int from 0 to 100.
  return lost;
}

int Player_city::get_food_production()
{
  int ret = 0;
  std::vector<Crop_amount> crops_grown = get_crops_grown();
  for (int i = 0; i < crops_grown.size(); i++) {
    Crop_amount crop = crops_grown[i];
    ret += crop.amount * Crop_data[crop.type]->food;
  }
// Crop_data[]->food is per 100 units of the crop, so naturally we must divide
// by 100.
  ret /= 100;
  return ret;
}

std::vector<Crop_amount> Player_city::get_crops_grown()
{
  std::vector<Crop_amount> ret;
// Look for any areas with buildings that provide RES_FARMING
  for (int i = 0; i < areas.size(); i++) {
    Building* build = &(areas[i].building);
    if (build->workers > 0 && areas[i].produces_resource(RES_FARMING)) {
      for (int n = 0; n < build->crops_grown.size(); n++) {
// Check if we already have that crop in ret
        if (build->crops_grown[n].amount > 0) {
          Crop_amount crop = build->crops_grown[n];
          crop.amount *= build->field_output;
          bool found_crop = false;
          for (int m = 0; !found_crop && m < ret.size(); m++) {
            if (ret[m].type == crop.type) {
              found_crop = true;
              ret[m].amount += crop.amount;
            }
          }
          if (!found_crop) { // Didn't combine it, so add it to the list
            ret.push_back( crop );
          }
        }
      }
    } // if (build->workers > 0 && areas[i].produces_resource(RES_FARMING))
  } // for (int i = 0; i < areas.size(); i++)

// TODO: Buildings (not from areas) that provide RES_FARMING?

  return ret;
}

// mineral defaults to MINERAL_NULL
int Player_city::get_amount_mined(Mineral mineral)
{
  std::map<Mineral,int> all_minerals = get_minerals_mined();
  if (mineral == MINERAL_NULL) {
    int ret = 0;
    for (std::map<Mineral,int>::iterator it = all_minerals.begin();
         it != all_minerals.end();
         it++) {
      ret += it->second;
    }
    return ret;
  }
  if (all_minerals.count(mineral)) {
    return all_minerals[mineral];
  }
  return 0;
}

std::map<Mineral,int> Player_city::get_minerals_mined()
{
  std::map<Mineral,int> ret;
// Look for any buildings that provide RES_MINING
  for (int i = 0; i < areas.size(); i++) {
    Building* build = &(areas[i].building);
    if (build->workers > 0 && areas[i].produces_resource(RES_MINING)) {
      for (int n = 0; n < build->minerals_mined.size(); n++) {
// Add each mineral to ret
        if (build->minerals_mined[n].amount > 0) {
          Mineral_amount mineral = build->minerals_mined[n];
          mineral.amount *= build->shaft_output;
          if (ret.count(mineral.type)) {
            ret[mineral.type] += mineral.amount;
          } else {
            ret[mineral.type] = mineral.amount;
          }
        }
      }
    } // if (build->workers > 0 && areas[i].produces_resource(RES_MINING))
  } // for (int i = 0; i < areas.size(); i++)

  return ret;
}

// TODO: This function.
std::map<Mineral,int> Player_city::get_minerals_used()
{
  std::map<Mineral,int> ret;
  return ret;
}

Animal_action Player_city::get_hunting_action(Animal animal)
{
  return hunting_action[animal];
}

void Player_city::set_hunting_action(Animal animal, Animal_action action)
{
  hunting_action[animal] = action;
}

void Player_city::do_hunt(Area* hunting_camp)
{
  if (!hunting_camp) {
    debugmsg("Player_city::do_hunt(NULL) called!");
    return;
  }

  Map_tile* tile = map.get_tile(hunting_camp->pos);
  if (!tile) {
    debugmsg("BUG - Hunting on NULL ground!");
    return;
  }

  Building* camp_bldg = &(hunting_camp->building);

  int hunters = camp_bldg->workers;
  int num_hunts = camp_bldg->amount_produced(RES_HUNTING) * hunters;

  if (num_hunts <= 0) {
    return;
  }

  int combat_points = camp_bldg->hunter_level;
  int hunter_hp = Race_data[race]->hp;
  int skill_level = Race_data[race]->skill_level[SKILL_HUNTING];

// These are for adding a message at the bottom of the function.
  std::vector<Animal_amount> animals_killed, animals_caught;

  for (int i = 0; camp_bldg->workers > 0 && i < num_hunts; i++) {
    Animal prey = tile->choose_hunt_animal(skill_level);
    Animal_datum* prey_dat = Animal_data[prey];
    int pack_size = 1;
    if (rng(1, 100) <= prey_dat->pack_chance) {
      pack_size = rng(1, prey_dat->max_pack_size);
    }

    if (prey != ANIMAL_NULL) {  // Ensure we actually caught something!

      bool combat = true;  // If we flee, set this to false; if true, do combat

// If we want to flee, try that first.
      if (hunting_action[prey] == ANIMAL_ACT_FLEE) {
// Affect chance of fleeing based on pack size.
        int flee_chance = prey_dat->flee_chance;
        flee_chance = (flee_chance * 20) / (19 + pack_size);
        bool fled = (rng(1, 100) <= prey_dat->flee_chance);

        if (skill_level < 3) {  // 1 or 2 extra chances to FAIL
          for (int n = 0; fled && n < 3 - skill_level; n++) {
            fled = rng(1, 100) <= prey_dat->flee_chance;
          }

        } else if (skill_level > 3) { // 1 or 2 extra chances to SUCCEED
          for (int n = 0; !fled && n < skill_level - 3; n++) {
            fled = rng(1, 100) <= prey_dat->flee_chance;
          }
        }

        if (fled) {  // Avoided combat!
          combat = false;
        }
      } // if (hunting_action[prey] == ANIMAL_ACT_FLEE)

      if (combat) {
// "2" limits us to 2 hunters fighting at once
        Battle_result result = quick_battle(hunters, combat_points, hunter_hp,
                                            pack_size, prey_dat->danger,
                                            prey_dat->hp);
        
        if (result.result == COMBAT_RES_ATTACKER_WON) { // We won!
// If we want to, try to capture it.
          bool caught = false;
          int num_caught = 0;
          if (hunting_action[prey] == ANIMAL_ACT_CAPTURE) {
// Try to catch each one seperately.
            for (int n = 0; n < pack_size; n++) {
// We use a mix of livestock & hunting skills for capturing things
              int livestock_skill =
                Race_data[race]->skill_level[SKILL_LIVESTOCK];
              int capture_skill = (livestock_skill * 2 + skill_level) / 3;
              caught = (rng(1, 100) + rng(0, 5 * n) <= prey_dat->tameness);
              if (capture_skill < 3) {  // 1 or 2 extra chances to FAIL
                for (int n = 0; caught && n < 3 - capture_skill; n++) {
                  caught = rng(1, 100) + rng(0, 5 * n) <= prey_dat->tameness;
                }
              } else if (capture_skill > 3) { // 1 or 2 extra chances to SUCCEED
                for (int n = 0; !caught && n < capture_skill - 3; n++) {
                  caught = rng(1, 100) + rng(0, 5 * n) <= prey_dat->tameness;
                }
              }

              if (caught) {
                num_caught++;
// Add them to our livestock
                if (livestock.count(prey)) {
                  livestock[prey]++;
                } else {
                  livestock[prey] = 1;
                }
              }
            }
          }
          if (caught) {
// Add us to the list of capture animals
            bool found = false;
            for (int n = 0; !found && n < animals_caught.size(); n++) {
              if (animals_caught[n].type == prey) {
                found = true;
                animals_caught[n].amount++;
              }
            }
            if (!found) {
              animals_caught.push_back( Animal_amount(prey, 1) );
            }
          } else {
            bool found = false;
            for (int n = 0; !found && n < animals_killed.size(); n++) {
              if (animals_killed[n].type == prey) {
                found = true;
                animals_killed[n].amount++;
              }
            }
            if (!found) {
              animals_killed.push_back( Animal_amount(prey, 1) );
            }

            kill_animals(prey, pack_size, hunting_camp->pos);
          }
        } // if (result.result == COMBAT_RES_ATTACKER_WON)

// Even if we won, some hunters may have died.
        if (result.attackers_dead > 0) {
// Remove them from this building.
          fire_citizens(CIT_PEASANT, result.attackers_dead, camp_bldg);
          std::stringstream death_reason;
          if (result.attackers_dead > 1) {
            death_reason << "were ";
          } else {
            death_reason << "was ";
          }
          death_reason << "killed by ";
          if (pack_size > 1) {
            death_reason << "a pack of " << prey_dat->name_plural;
          } else {
            death_reason << "a " << prey_dat->name;
          }
          kill_citizens(CIT_PEASANT, result.attackers_dead, DEATH_HUNTING,
                        death_reason.str());
        }
      } // if (combat)
    } // if (prey != ANIMAL_NULL)
  } // for (int i = 0; i < num_hunts; i++)

// Now, add a message about what we killed/caught.
  std::stringstream ss_message;
  ss_message << "Hunt result: ";
  if (animals_killed.empty() && animals_caught.empty()) {
    ss_message << "No animals killed or caught.";
  }

  if (!animals_killed.empty()) {

    for (int i = 0; i < animals_killed.size(); i++) {
// Conjunction (or comma)
      Animal_datum* animal_dat = Animal_data[ animals_killed[i].type ];
      if (i > 0) {
        if (i == animals_killed.size() - 1) {
          ss_message << " and ";
        } else {
          ss_message << ", ";
        }
      }

// Article (or number)
      if (animals_killed[i].amount == 1) {
        ss_message << "a";
      } else {
        ss_message << animals_killed[i].amount;
      }

      ss_message << " ";
      if (animals_killed[i].amount == 1) {
        ss_message << animal_dat->name;
      } else {
        ss_message << animal_dat->name_plural;
      }
    }
// Verb
    if (animals_killed.size() > 1 || animals_killed[0].amount > 1) {
      ss_message << " were ";
    } else {
      ss_message << " was ";
    }
    ss_message << "killed";
// Do we need to link to animals_caught?
    if (animals_caught.empty()) { // Just end the sentence.
      ss_message << ".";
    } else {  // Continue the sentence with a description of the animals caught.
      ss_message << ", and ";
    }
  }

  if (!animals_caught.empty()) {

    for (int i = 0; i < animals_caught.size(); i++) {
// Conjunction (or comma)
      Animal_datum* animal_dat = Animal_data[ animals_caught[i].type ];
      if (i > 0) {
        if (i == animals_caught.size() - 1) {
          ss_message << " and ";
        } else {
          ss_message << ", ";
        }
      }

// Article (or number)
      if (animals_caught[i].amount == 1) {
        ss_message << "a";
      } else {
        ss_message << animals_caught[i].amount;
      }

      ss_message << " ";
      if (animals_caught[i].amount == 1) {
        ss_message << animal_dat->name;
      } else {
        ss_message << animal_dat->name_plural;
      }
    }
// Verb
    if (animals_caught.size() > 1 || animals_caught[0].amount > 1) {
      ss_message << " were ";
    } else {
      ss_message << " was ";
    }
    ss_message << "captured.";
  }

  std::string message = capitalize( ss_message.str() );

  add_message(MESSAGE_MINOR, message);

}

void Player_city::kill_animals(Animal animal, int amount, Point pos)
{
  Animal_datum* animal_dat = Animal_data[animal];

  gain_resource(RES_FOOD, amount * animal_dat->food_killed);

  for (int i = 0; i < animal_dat->resources_killed.size(); i++) {
    Resource_amount res = animal_dat->resources_killed[i];
    res.amount *= amount;
    gain_resource(res);
  }

// pos defaults to (-1, -1) which means the animal didn't come from the map.
// Otherwise, the animal came from the map, while hunting.
  Map_tile* tile = map.get_tile(pos);
  if (tile) {
// If we have a tile, this was a hunt kill; so add the animals to our records.
    if (hunt_kills.count(animal)) {
      hunt_kills[animal] += amount;
    } else {
      hunt_kills[animal] = amount;
    }

// Remove animals from the map.
    bool done = false;
    for (int i = 0; !done && i < tile->animals.size(); i++) {
      if (tile->animals[i].type == animal) {
        done = true;
        tile->animals[i].amount -= amount;
        if (tile->animals[i].amount <= 0) {
          tile->animals.erase(tile->animals.begin() + i);
        }
      }
    }
  }
}

// TODO: This function.
int Player_city::get_import(Resource res)
{
  return 0;
}

// TODO: This function.
int Player_city::get_export(Resource res)
{
  return 0;
}

nc_color message_type_color(Message_type type)
{
  switch (type) {
    case MESSAGE_NULL:    return c_dkgray;
    case MESSAGE_MINOR:   return c_ltgray;
    case MESSAGE_MAJOR:   return c_yellow;
    case MESSAGE_URGENT:  return c_ltred;
    case MESSAGE_MAX:     return c_dkgray;
    default:              return c_magenta;
  }
  return c_magenta;
}

City_type lookup_city_type(std::string name)
{
  name = no_caps( trim( name ) );
  for (int i = 0; i < CITY_TYPE_MAX; i++) {
    City_type ret = City_type(i);
    if (name == no_caps( trim( city_type_name(ret) ) ) ) {
      return ret;
    }
  }
  return CITY_TYPE_NULL;
}

std::string city_type_name(City_type type)
{
  switch (type) {
    case CITY_TYPE_NULL:    return "NULL";
    case CITY_TYPE_CITY:    return "city";
    case CITY_TYPE_DUCHY:   return "duchy seat";
    case CITY_TYPE_CAPITAL: return "capital";
    case CITY_TYPE_MAX:     return "BUG - CITY_TYPE_MAX";
    default:                return "BUG - Unnamed City_type";
  }
  return "BUG - Escaped city_type_name() switch!";
}
