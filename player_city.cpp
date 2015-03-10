#include "player_city.h"
#include "cuss.h"
#include "window.h"
#include "building.h"
#include "stringfunc.h"
#include "geometry.h"
#include "rng.h"
#include "combat.h" // For hunting
#include "globals.h"
#include "kingdom.h"
#include <sstream>
#include <vector>
#include <map>

std::string Message::save_data()
{
  std::stringstream ret;
  ret << int(type) << " ";
  ret << date.save_data() << std::endl;
// Since text is probably multi-word, we use ! as a terminator.
  ret << text << " ! ";

  return ret.str();
}

bool Message::load_data(std::istream& data)
{
  int tmptype;
  data >> tmptype;
  if (tmptype <= 0 || tmptype >= MESSAGE_MAX) {
    debugmsg("Message loaded type %d (range is 1 to %d).",
             tmptype, MESSAGE_MAX - 1);
    return false;
  }
  type = Message_type(tmptype);

  if (!date.load_data(data)) {
    debugmsg("Message failed to load its date.");
    return false;
  }

  std::string word;
  while (word != "!") {
    data >> word;
    if (word != "!") {
      if (!text.empty()) {
        text = text + " ";
      }
      text = text + word;
    }
  }

  return true;
}

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

  for (int i = 0; i < AREA_MAX; i++) {
    Area_datum* area_dat = Area_data[i];
// If the area is NOT unlockable, that means it starts out unlocked!
    area_unlocked[i] = !(area_dat->unlockable);
  }

  for (int i = 0; i < BUILD_MAX; i++) {
    Building_datum* build_dat = Building_data[i];
// If the building is NOT unlockable, that means it starts out unlocked!
    building_unlocked[i] = !(build_dat->unlockable);
  }

  radius = 1;
  unread_messages = 0;
  show_hunting_messages = true;
  show_livestock_messages = true;

  hunt_record_days = -1;  // We set it to 0 when we build our first camp
  hunt_record_food = 0;
}

Player_city::~Player_city()
{
}

std::string Player_city::save_data()
{
  std::stringstream ret;

  ret << City::save_data() << std::endl;

  for (int i = 0; i < CIT_MAX; i++) {
    ret << tax_rate[i] << " ";
  }
  ret << std::endl;

  ret << units_stationed.size() << " ";
  for (int i = 0; i < units_stationed.size(); i++) {
    ret << units_stationed[i].save_data() << " ";
  }
  ret << std::endl;

  ret << radius << std::endl;

  for (int i = 0; i < AREA_MAX; i++) {
    ret << area_unlocked[i] << " ";
  }
  ret << std::endl;

  for (int i = 0; i < BUILD_MAX; i++) {
    ret << building_unlocked[i] << " ";
  }
  ret << std::endl;

  ret << buildings.size() << " ";
  for (int i = 0; i < buildings.size(); i++) {
    ret << buildings[i].save_data() << " ";
  }
  ret << std::endl;

  ret << building_queue.size() << " ";
  for (int i = 0; i < building_queue.size(); i++) {
    ret << building_queue[i].save_data() << " ";
  }
  ret << std::endl;

  ret << areas.size() << " ";
  for (int i = 0; i < areas.size(); i++) {
    ret << areas[i].save_data() << " ";
  }
  ret << std::endl;

  ret << area_queue.size() << " ";
  for (int i = 0; i < area_queue.size(); i++) {
    ret << area_queue[i].save_data() << " ";
  }
  ret << std::endl;

  ret << hunt_record_days << " ";
  ret << hunt_record_food << " ";
  ret << hunt_kills.size() << " ";
  for (std::map<Animal,int>::iterator it = hunt_kills.begin();
       it != hunt_kills.end();
       it++) {
    ret << it->first << " " << it->second << " ";
  }
  ret << std::endl;

  ret << unread_messages << " ";
  ret << show_hunting_messages << " ";
  ret << show_livestock_messages << " ";
  ret << messages.size() << " ";
  for (int i = 0; i < messages.size(); i++) {
    ret << messages[i].save_data() << std::endl;
  }
  ret << std::endl;

  ret << world_seen.save_data() << std::endl;

  ret << birth_points << std::endl;

  for (int i = 0; i < ANIMAL_MAX; i++) {
    ret << hunting_action[i] << " ";
  }

  return ret.str();
}

bool Player_city::load_data(std::istream& data)
{
  if (!City::load_data(data)) {
    debugmsg("Player_city::load_data() failed when calling City::load_data().");
    return false;
  }

  for (int i = 0; i < CIT_MAX; i++) {
    data >> tax_rate[i];
  }

  int num_units;
  data >> num_units;
  for (int i = 0; i < num_units; i++) {
    Military_unit tmp_unit;
    if (!tmp_unit.load_data(data)) {
      debugmsg("Player_city failed to load Military_unit %d/%d.",
               i, num_units);
      return false;
    }
    units_stationed.push_back(tmp_unit);
  }

  data >> radius;

  for (int i = 0; i < AREA_MAX; i++) {
    data >> area_unlocked[i];
  }
  for (int i = 0; i < BUILD_MAX; i++) {
    data >> building_unlocked[i];
  }

  int num_buildings;
  data >> num_buildings;
  for (int i = 0; i < num_buildings; i++) {
    Building tmp_build;
    if (!tmp_build.load_data(data)) {
      debugmsg("Player_city failed to load building %d/%d.", i, num_buildings);
      return false;
    }
    buildings.push_back(tmp_build);
  }

  int num_bldg_queue;
  data >> num_bldg_queue;
  for (int i = 0; i < num_bldg_queue; i++) {
    Building tmp_build;
    if (!tmp_build.load_data(data)) {
      debugmsg("Player_city failed to load queued building %d/%d.",
               i, num_buildings);
      return false;
    }
    building_queue.push_back(tmp_build);
  }

  int num_areas;
  data >> num_areas;
  for (int i = 0; i < num_areas; i++) {
    Area tmp_area;
    if (!tmp_area.load_data(data)) {
      debugmsg("Player_city failed to load area %d/%d.", i, num_areas);
      return false;
    }
    areas.push_back(tmp_area);
  }

  int num_area_queue;
  data >> num_area_queue;
  for (int i = 0; i < num_area_queue; i++) {
    Area tmp_area;
    if (!tmp_area.load_data(data)) {
      debugmsg("Player_city failed to load area %d/%d.", i, num_areas);
      return false;
    }
    areas.push_back(tmp_area);
  }

  int num_kills;
  data >> hunt_record_days >> hunt_record_food >> num_kills;
  for (int i = 0; i < num_kills; i++) {
    int tmpani, tmpnum;
    data >> tmpani >> tmpnum;
    if (tmpani <= 0 || tmpani >= ANIMAL_MAX) {
      debugmsg("Player_city loaded hunt_kills on animal %d (range is 1 to %d).",
               tmpani, ANIMAL_MAX - 1);
      return false;
    }
    hunt_kills[ Animal(tmpani) ] = tmpnum;
  }

  int num_messages;
  data >> unread_messages >> show_hunting_messages >> show_livestock_messages >>
          num_messages;
  for (int i = 0; i < num_messages; i++) {
    Message tmpmes;
    if (!tmpmes.load_data(data)) {
      debugmsg("Player_city failed to load message %d/%d.", i, num_messages);
      return false;
    }
    messages.push_back(tmpmes);
  }

  if (!world_seen.load_data(data)) {
    debugmsg("Player_city failed to load Map_seen.");
    return false;
  }

  data >> birth_points;

  for (int i = 0; i < ANIMAL_MAX; i++) {
    int tmpact;
    data >> tmpact;
    if (tmpact <= 0 || tmpact >= ANIMAL_ACT_MAX) {
      debugmsg("Player_city loaded hunting_act %d/%d; %d (range is 1 to %d).",
               i, ANIMAL_MAX, tmpact, ANIMAL_ACT_MAX - 1);
      return false;
    }
    hunting_action[i] = Animal_action(tmpact);
  }

  return true;
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

void Player_city::set_name()
{
  cuss::interface i_name;
  if (!i_name.load_from_file("cuss/set_name.cuss")) {
    name = "ERRORTOWN";
    return;
  }

  Window w_name;

  i_name.select("entry_name");
  i_name.ref_data("entry_name", &name);
  bool done = false;

  while (!done) {
    i_name.draw(&w_name);
    w_name.refresh();

    long ch = input();

    if (ch == '!') {
      set_random_name();

    } else if (ch == '\n') {
      done = true;

    } else {
      i_name.handle_keypress(ch);
    }
  }
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

void Player_city::set_starting_tiles_seen()
{
  Kingdom* kingdom = GAME->get_kingdom_for_race(race);
  if (!kingdom) {
    debugmsg("No kingdom found for Player_city::set_starting_tiles_seen().");
    return;
  }
// TODO: Make the bonus area vary by some property of our race?
// TODO: Round the corners of the square this produces.
  int bonus = 4;
/*
  for (int x = kingdom->most_west - bonus;
           x <= kingdom->most_east + bonus;
           x++) {
    for (int y = kingdom->most_north - bonus;
             y <= kingdom->most_south + bonus;
             y++) {
      world_seen.mark_seen(x, y);
    }
  }
*/
  for (int x = kingdom->most_west; x <= kingdom->most_east; x++) {
    for (int y = kingdom->most_north; y <= kingdom->most_south; y++) {
      int id = GAME->world->get_kingdom_id(x, y);
      if (id == kingdom->uid) {
        for (int mx = x - bonus; mx <= x + bonus; mx++) {
          for (int my = y - bonus; my <= y + bonus; my++) {
            world_seen.mark_seen(mx, my);
          }
        }
      }
    }
  }

// Go further afield, but only for oceans!
  bonus = bonus * 10;
  for (int x = kingdom->most_west - bonus;
           x <= kingdom->most_east + bonus;
           x++) {
    for (int y = kingdom->most_north - bonus;
             y <= kingdom->most_south + bonus;
             y++) {
      Map_type mt = GAME->world->get_map_type(x, y);
      if (mt == MAP_OCEAN || mt == MAP_ICECAP) {
        for (int mx = x - 1; mx <= x + 1; mx++) {
          for (int my = y - 1; my <= y + 1; my++) {
            world_seen.mark_seen(mx, my);
          }
        }
      }
    }
  }
}

void Player_city::mark_nearby_tiles_seen(int range)
{
  for (int x = location.x - range; x <= location.x + range; x++) {
    for (int y = location.y - range; y <= location.y + range; y++) {
      world_seen.mark_seen(x, y);
    }
  }
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
      } else if (!area->is_open()) {
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
// Reduce morale modifiers for all citizens, and consume luxuries.
  for (int i = CIT_PEASANT; i <= CIT_BURGHER; i++) {
    population[i].decrease_morale_mods();
  }
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
    int dead = 0;
    if (starve_chance >= 100) {
      dead = population[i].count; // Game over, man!
    } else if (starve_chance > 0) {
      for (int n = 0; n < population[i].count; n++) {
        if (rng(1, 100) <= starve_chance) {
          dead++;
        }
      }
    }
// Lose morale, even if no one died.
    int morale_penalty = (0 - starve_chance) / 10 - 3 * dead;
    population[i].add_morale_modifier(MORALE_MOD_HUNGER, morale_penalty);
    kill_citizens( Citizen_type(i), dead, DEATH_STARVATION );
  } // for (int i = 0; i < CIT_MAX; i++)

// Handle livestock.
  handle_livestock();

// Handle areas & buildings: do_production() does innate resource production,
// manufacturing, farming, mining, and logging.  We also call do_hunt() for
// hunting camps and discover_minerals() for mines.
  for (int i = 0; i < areas.size(); i++) {
    Building* build = &(areas[i].building);
    Building_datum* build_dat = build->get_building_datum();

    build->do_production(this);

    if (build_dat->base_morale > 0) {
// Give all our citizens its morale
      for (int n = CIT_PEASANT; n <= CIT_BURGHER; n++) {
        population[n].add_morale_modifier(MORALE_MOD_BUILDING,
                                          build_dat->base_morale * 10,
                                          build->get_name());
      }
    }
    if (build->produces_resource(RES_HUNTING)) {
      do_hunt( &(areas[i]) );
    }
    if (build->produces_resource(RES_MINING)) {
      build->discover_minerals(this);
    }
  }

  for (int i = 0; i < buildings.size(); i++) {
    Building_datum* build_dat = buildings[i].get_building_datum();

    buildings[i].do_production(this);

    if (build_dat->base_morale > 0) {
// Give all our citizens its morale
      for (int n = CIT_PEASANT; n <= CIT_BURGHER; n++) {
        population[n].add_morale_modifier(MORALE_MOD_BUILDING,
                                          build_dat->base_morale * 10,
                                          buildings[i].get_name());
      }
    }
  }

// Increment hunt_record_days.
  if (hunt_record_days >= 0) {
    hunt_record_days++;
  }

// Consume food
  feed_citizens();

// Pay wages.
  int wages = get_total_wages();
  if (!expend_resource(RES_GOLD, wages)) {
// TODO: Consequences for failure to pay wages!
    resources[RES_GOLD] = 0;
  }

// Lose gold to corruption.
  int corruption = get_corruption_amount();
  if (!expend_resource(RES_GOLD, corruption)) {
// TODO: Consequences for failure to pay corruption?
    resources[RES_GOLD] = 0;
  }

// We total maintenance into a single pool because we'll need to divide the gold
// by 10, and we want to lose as little to rounding as possible.
  std::map<Resource,int> total_maintenance;
// Deduct maintenance for all areas
  for (int i = 0; i < areas.size(); i++) {
    if (areas[i].is_open()) {
      std::map<Resource,int> maintenance = areas[i].get_maintenance();
      for (std::map<Resource,int>::iterator it = maintenance.begin();
           it != maintenance.end();
           it++) {
        total_maintenance[it->first] += it->second;
      }
    }
  }
// Deduct maintenance for all buildings
  for (int i = 0; i < buildings.size(); i++) {
    if (buildings[i].open) {
      std::map<Resource,int> maintenance = buildings[i].get_maintenance();
      for (std::map<Resource,int>::iterator it = maintenance.begin();
           it != maintenance.end();
           it++) {
        total_maintenance[it->first] += it->second;
      }
    }
  }
  if (total_maintenance.count(RES_GOLD)) {
    total_maintenance[RES_GOLD] /= 10;
  }
  if (!expend_resources(total_maintenance)) {
// TODO: Close some areas until we CAN pay this.
  }

// Let citizens consume luxuries.
  for (int i = CIT_PEASANT; i <= CIT_BURGHER; i++) {
    population[i].consume_luxuries(this);
  }

// The last resource transaction we should do is exporting resources, since it's
// presumably the one with the least consequences.
  for (int i = 1; i < RES_MAX; i++) {
    Resource res_export = Resource(i);
    int export_amt = get_export(res_export);
    if (resources[res_export] >= export_amt) {
      resources[res_export] -= export_amt;
    } else {
// TODO: consequences for failure to export
    }
  }

// Make sure our food isn't above the cap (food goes bad y'know)
  int food_cap = get_food_cap();
  if (resources[RES_FOOD] > food_cap) {
    resources[RES_FOOD] = food_cap;
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
    if (building_to_build->construction_left <= 0) {
      add_message(MESSAGE_MINOR, "Our %s has finished construction.",
                  building_queue[0].get_name().c_str());
      add_open_building(building_queue[0]);
      building_queue.erase( building_queue.begin() );
    }
  }

// Check if we've unlocked any new areas, buildings, etc.
  check_unlockables();
}

void Player_city::handle_livestock()
{
  for (std::map<Animal,int>::iterator it = livestock.begin();
       it != livestock.end();
       it++) {
    int skill = Race_data[race]->skill_level[SKILL_LIVESTOCK];
    Animal animal = it->first;
    Animal_datum* animal_dat = Animal_data[animal];
    int animal_amount = it->second;

// Now we make any daily food gains (e.g. milk, eggs, etc)
    int food_made = animal_dat->food_livestock;
    if (food_made > 0) {
// food_livestock is per 100 animals!
      int food_gained = (animal_amount * food_made) / 100;
// Round randomly.
      if (rng(1, 100) >= (animal_amount * food_made) % 100) {
        food_gained++;
      }
// Adjust based on our livestock skill.
      food_gained = (food_gained * (5 + skill)) / 8;
      gain_resource(RES_FOOD, food_gained);
    }

// Other daily resource gains.
    for (int i = 0; i < animal_dat->resources_livestock.size(); i++) {
      Resource_amount res = animal_dat->resources_livestock[i];
      int orig_amount = res.amount; // For rounding, below.
// Again, the amount is per 100 animals.
      res.amount = (animal_amount * res.amount) / 100;
// Round randomly.
      if (rng(1, 100) >= (animal_amount * orig_amount) % 100) {
        res.amount++;
      }
// Adjust based on our livestock skill.
      res.amount = (res.amount * (7 + skill)) / 10;
      gain_resource(res);
    }

// Do any die randomly?
    int num_died = 0;
    for (int i = 0; i < animal_amount; i++) {
      int hardiness = animal_dat->hardiness;
// Adjust based on our livestock skill.
      hardiness = (hardiness * (7 + skill)) / 10;
      if (one_in(hardiness)) {
        num_died++;
      }
    }

    if (num_died > 0) {
      it->second -= num_died;
      if (it->second < 0) { // Shouldn't happen but let's be safe
        it->second = 0;
      }
      animal_amount = it->second;
// At least we get to slaughter them for food!
      int food_gain = num_died * animal_dat->food_killed;
      kill_animals(animal, num_died);
// Add a message about it.
      if (show_livestock_messages) {
        std::stringstream ss_mes;
        ss_mes << num_died << " ";
        if (num_died == 1) {
          ss_mes << animal_dat->name << " has died naturally.  It was ";
        } else {
          ss_mes << animal_dat->name_plural << " have died naturally.  They " <<
                    "were ";
        }
        ss_mes << "butchered for " << food_gain << " food.";
        add_message(MESSAGE_MINOR, ss_mes.str());
      }
    }

// Finally, check to see if any were born.
    int num_born = 0;
    for (int i = 0; i < animal_amount; i++) {
      int repro_chance = animal_dat->reproduction_rate;
// Adjust based on our livestock skill.
      repro_chance = (repro_chance * 10) / (7 + skill);
      if (one_in(repro_chance)) {
        num_born++;
      }
    }

    if (num_born > 0) {
      int size = animal_dat->size;
      int new_total = get_livestock_total() + num_born * size;
      int capacity = get_livestock_capacity();
      int overflow = 0;
// num_born is for the message, real_num_born is added to the population
// (it may change if we overflow our capacity)
      int real_num_born = num_born;
      if (new_total >= capacity) {
// Oh no, we're over capacity!
// Subtracting 1 means that if new_total == capacity we round down to 0.
// Then we add 1, so we're always rounding up.
        overflow = 1 + (new_total - capacity - 1) / size;
// Kill the overflow rather than just setting them loose.
        kill_animals(animal, overflow);
        real_num_born -= overflow;
      }

// Add a message.
      if (show_livestock_messages) {
        std::stringstream ss_mes;
        ss_mes << num_born << " ";
        if (num_born == 1) {
          ss_mes << animal_dat->name << " was born!";
        } else {
          ss_mes << animal_dat->name_plural << " were born!";
        }
        add_message(MESSAGE_MINOR, ss_mes.str());
        if (overflow > 0) {
          std::stringstream ss_overflow;
          ss_overflow << overflow << " ";
          if (overflow == 1) {
            ss_overflow << animal_dat->name;
          } else {
            ss_overflow << animal_dat->name_plural;
          }
          ss_overflow << " had to be slaughtered to make room.",
          add_message(MESSAGE_MAJOR, ss_overflow.str());
        }
      }
      if (real_num_born > 0) {
        it->second += real_num_born;
      }
    } // if (num_born > 0)
  } // for (std::map<Animal,int>::iterator it = livestock.begin(); .....
}

void Player_city::feed_citizens()
{
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
// Multiply food_deficit by 100, since citizen_consumption is per 100 citizens
          hungry_citizens = (100 * food_deficit) / citizen_consumption;
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
      } // resources[RES_FOOD] < type_consumption
    } // for (int i = CIT_MAX - 1; i > CIT_NULL; i--)
  } // resources[RES_FOOD] < food_consumed
}

void Player_city::check_unlockables()
{
// First, do areas
  for (int i = 0; i < AREA_MAX; i++) {
// Only check if it's not already unlocked.
    if (!area_unlocked[i]) {
      Area_datum* area_dat = Area_data[i];
      City_achievement achievement = area_dat->unlock_condition;

      if (meets_achievement( achievement ) ) {
// We did it!
        area_unlocked[i] = true;
        add_message(MESSAGE_UNLOCK, "New area unlocked: %s!",
                    area_dat->name.c_str());
      }
    }
  }

// Now, buildings
  for (int i = 0; i < BUILD_MAX; i++) {
// Only check if it's not already unlocked.
    if (!building_unlocked[i]) {
      Building_datum* build_dat = Building_data[i];
      City_achievement achievement = build_dat->unlock_condition;

      if (meets_achievement( achievement ) ) {
// We did it!
        building_unlocked[i] = true;
        add_message(MESSAGE_UNLOCK, "New building unlocked: %s!",
                    build_dat->name.c_str());
      }
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
  area.building.open = true;
// Figure out how many crops per field we get
  Building_datum* build_dat = area.get_building_datum();
  if (!build_dat) {
    debugmsg("NULL Building_data* in Player_city::open_area (%s).",
             area.get_name().c_str());
  }

// Farms are set up specially.
// Check if this area produces RES_FARMING.
  int farming = build_dat->amount_produced(RES_FARMING);
  if (farming > 0) {
    Map_tile* tile_here = map.get_tile(area.pos);
    Building* farm_bld = &(area.building);
    farming *= tile_here->get_farmability();
// Alter farming based on racial ability.
// Skill level of 5 = no reduction, 1 = 1/5th of the normal rate.
    farming *= Race_data[race]->skill_level[SKILL_FARMING];
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
  int mining = build_dat->amount_produced(RES_MINING);
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
// Start recording hunting food if we haven't already
    if (hunt_record_days == -1) {
      hunt_record_days = 0;
    }
  }

// Automatically hire workers and set crops/minerals
  area.auto_hire(this);

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
  building.open = true; // Just in case
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
/* TODO: If there's 3 slots available, and we try to hire 4, the function
 *       returns false and doesn't employ any.  Is this good?  I think so;
 *       sometimes we rely on this function to actually employ the exact number
 *       of citizens specified, so we shouldn't subvert that assumption.  If
 *       needed we can add a flag to this function, or write a seperate
 *       function, which hires as many as possible (up to a specified limit).
 */
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
// Do we need this check?  It doesn't seem like it'd hurt...
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

// Morale modifiers.  Citizens of the class that died get it slightly worse.
  population[type].add_morale_modifier(MORALE_MOD_DEATH, -20);
  for (int i = CIT_PEASANT; i <= CIT_BURGHER; i++) {
    population[i].add_morale_modifier(MORALE_MOD_DEATH, -30);
  }
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
  new_message.date = GAME->get_date();

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
    } else if (!area->is_open()) {
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
    if (buildings[i].open && build_dat) {
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
    if (buildings[i].open && build_dat) {
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
    return -1;
  }

  int ratio = get_required_ratio(cit_type);
  if (ratio == 0) {
    return -1;
  }

  int type_below = cit_type - 1;
  return population[type_below].count / ratio;
}

int Player_city::get_required_morale(Citizen_type cit_type)
{
  return Race_data[race]->morale_requirement[cit_type];
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

// Start from 100 percent
  int ret = 100;

// Are we below required morale?
  int morale_req = get_required_morale(cit_type);
  int morale_had = population[lower_class].get_morale_percentage();
  if (morale_had < morale_req) {
// Lose up to 80%!
    ret = ret - 80 + (80 * morale_had) / morale_req;
  }

  return ret;
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
    if (new_cit_type == CIT_NULL) {
      new_cit_type = CIT_PEASANT; // Birth peasants by default.
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
  Message_type mtype = MESSAGE_MINOR;
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

// If this was the first citizen of that class, the player probably wants to
// know that they can start using buildings/etc that need that class.
    if (population[ born[i].type ].count == born[i].amount) {
      mtype = MESSAGE_MAJOR;
    }
  }
// Verb
  if (born.size() > 1 || born[0].amount > 1) {
    ss_message << " were born!";
  } else {
    ss_message << " was born!";
  }

  std::string message = capitalize( ss_message.str() );
  add_message( mtype, message );
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
    if (bldg->open && bldg->get_filled_jobs(cit_type) > 0) {
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

int Player_city::get_buildings_supported()
{
  int ret = 0;
  for (int i = 0; i < areas.size(); i++) {
    Area_datum* area_dat = areas[i].get_area_datum();
    ret += area_dat->buildings_supported;
  }

  return ret;
}

int Player_city::get_total_maintenance()
{
  int ret = 0;
  for (int i = 0; i < areas.size(); i++) {
    if (areas[i].is_open()) {
      ret += areas[i].get_building_datum()->upkeep;
    }
  }
  for (int i = 0; i < buildings.size(); i++) {
    if (buildings[i].open) {
      ret += buildings[i].get_building_datum()->upkeep;
    }
  }
  return ret / 10;  // Since maintenance is in 1/10th of a gold
}

int Player_city::get_fields_worked()
{
  int ret = 0;
  for (int i = 0; i < areas.size(); i++) {
    if (areas[i].is_open() && areas[i].type == AREA_FARM) {
      ret += areas[i].building.workers;
    }
  }
  return ret;
}

int Player_city::get_empty_fields()
{
  int ret = 0;
  for (int i = 0; i < areas.size(); i++) {
    if (areas[i].is_open() && areas[i].type == AREA_FARM) {
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
    if (areas[i].is_open() && areas[i].type == AREA_MINE) {
      ret += areas[i].building.workers;
    }
  }
  return ret;
}

int Player_city::get_free_shafts()
{
  int ret = 0;
  for (int i = 0; i < areas.size(); i++) {
    if (areas[i].is_open() && areas[i].type == AREA_MINE) {
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
    if (bldgs[i]->open) {
      ret += bldgs[i]->amount_produced(res);
      ret += bldgs[i]->amount_built(res, this);
    }
  }

  return ret;
}

int Player_city::get_gross_resource_production(Resource res)
{
  int ret = 0;
  std::vector<Building*> bldgs = get_all_buildings();
  for (int i = 0; i < bldgs.size(); i++) {
    std::map<Resource,int> produced = bldgs[i]->get_resource_production(this);
    ret += produced[res];
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
    if (buildings[i].open && workers > 0 &&
        (type == CIT_NULL || type == build_dat->jobs.type)) {
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
  if (rate < low_rate) {  // More than 25
    population[type].tax_morale = 25 + low_rate - rate;
  } else if (rate > high_rate) {  // Always -25, ouch!
    population[type].tax_morale = -25;
  } else {  // Between 0 and 20
    population[type].tax_morale = 20 - (20 * (rate - low_rate)) / high_rate;
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
  for (int i = 0; i < areas.size(); i++) {
    Building* build = &(areas[i].building);
    if (build->workers > 0 && areas[i].produces_resource(RES_FARMING)) {
      for (int n = 0; n < build->crops_grown.size(); n++) {
        int amount = build->crops_grown[n].amount;
        if (amount > 0) {
          Crop crop = build->crops_grown[n].type;
          Crop_datum* crop_dat = Crop_data[crop];
          int food = crop_dat->food;
          food *= amount;
          food *= build->field_output;
          ret += food;
        } // if (amount > 0)
      } // for (int n = 0; n < build->crops_grown.size(); n++)
    } // if (build->workers > 0 && areas[i].produces_resource(RES_FARMING))
  } // for (int i = 0; i < areas.size(); i++)

/* Crop_data[]->food is per 100 units of the crop, so naturally we must divide
 * by 100.  Additionally, Building::field_output is 100 times higher than it
 * should be (since terrain farmability is a percentage, 0 to 100) so we must
 * divide by 100.  Combining both means dividing by 10000.
 */
  ret /= 10000;
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

  if (hunters <= 0) {
    return;
  }

  Animal_action action = camp_bldg->hunting_action;
  int combat_points    = camp_bldg->hunter_level;
  int hunting_points   = camp_bldg->hunter_level * hunters *
                         camp_bldg->amount_produced(RES_HUNTING);

  int hunter_hp        = Race_data[race]->hp;
  int hunting_skill    = Race_data[race]->skill_level[SKILL_HUNTING];
  int livestock_skill  = Race_data[race]->skill_level[SKILL_LIVESTOCK];

// These are for adding a message at the bottom of the function.
// animals_both are for when we caught an animal but didn't have space for it,
// so we slaughtered it anyway.
  std::map<Animal,int> animals_killed, animals_caught, animals_both;

  while (hunting_points > 0) {
// Pick an animal; usually it'll be the one we've specified.
    Animal target = camp_bldg->hunting_target;
// 1 in 10 for skill 1, 1 in 32 for skill 3, 1 in 50 for skill 5.
    if (one_in(5 + hunting_skill * 9)) {
      target = tile->choose_hunt_animal(hunting_skill);
    }
    Animal_datum* target_data = Animal_data[target];
    int pack_size = 1;

    if (target != ANIMAL_NULL) {  // A randomized target MIGHT be null.
      if (target_data->pack_chance > 0 &&
          rng(1, 100) <= target_data->pack_chance) {
        pack_size = rng(1, target_data->max_pack_size);
      }

// Use up some of our hunting points
      int point_cost = target_data->difficulty;
      if (pack_size > 1) {
// Hunting a pack of N is faster than hunting N individuals.
        for (int n = 1; n < pack_size; n++) {
          point_cost += (n * target_data->difficulty) / (n + 2);
        }
      }
// Sanity check...
      if (point_cost <= 0) {
        debugmsg("Hunting cost 0! (%s, pack of %d)",
                 target_data->name.c_str(), pack_size);
        point_cost = 1;
      }
// If we don't have enough points, we roll against the cost.  If our roll is
// higher we get the animal; either way, we lose all our points.
      if (point_cost > hunting_points) {
        if (rng(0, point_cost) > rng(0, hunting_points)) {
          target = ANIMAL_NULL;
        }
      }
      hunting_points -= point_cost;
    } else { // if (target != ANIMAL_NULL)
      hunting_points -= 100;  // Time wasted
    }

// Check for ANIMAL_NULL again, since it may have changed in the above block.
    if (target != ANIMAL_NULL) {

// Now.... FIGHT!
      Battle_result result = quick_battle(hunters, combat_points, hunter_hp,
                                          pack_size, target_data->danger,
                                          target_data->hp);
      
      if (result.result == COMBAT_RES_ATTACKER_WON) { // We won!
// If we want to, try to capture it.
        bool caught = false;
        bool caught_and_killed = false; // If we don't have livestock space
        int num_caught = 0;
        if (action == ANIMAL_ACT_CAPTURE) {
// We use 2/3 livestock skill & 1/3 hunting skill for capturing things
          int capture_skill = (livestock_skill * 2 + hunting_skill) / 3;
// Try to catch each one seperately.
          for (int n = 0; n < pack_size; n++) {
// We add up to 5 points for each creature beyond the first; it's hard to corral
// an entire pack of wild animals!
            caught = (rng(1, 100) + rng(0, 5 * n) <= target_data->tameness);
            if (capture_skill < 3) {  // 1 or 2 extra chances to FAIL
              for (int m = 0; caught && m < 3 - capture_skill; m++) {
                caught = rng(1, 100) + rng(0, 5 * n) <= target_data->tameness;
              }
            } else if (capture_skill > 3) { // 1 or 2 extra chances to SUCCEED
              for (int m = 0; !caught && m < capture_skill - 3; m++) {
                caught = rng(1, 100) + rng(0, 5 * n) <= target_data->tameness;
              }
            }

            if (caught) {
// Add them to our livestock, if we can...
              int size = target_data->size;
              if (get_livestock_total() + size <= get_livestock_capacity()) {
                num_caught++;
                livestock[target]++;
              } else { // We can't hold any more livestock! Kill them instead :(
                caught_and_killed = true;
                kill_animals(target, 1, hunting_camp->pos);
              }
            }
          }
        }
// Add us to the appropriate list & kill if necessary
        if (caught_and_killed) {
          animals_both[target]++;
        } else if (caught) {
          animals_caught[target] += num_caught;
        } else {  // Not caught-and-killed, not caught - just killed
          animals_killed[target]++;
          kill_animals(target, pack_size, hunting_camp->pos);
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
          death_reason << "a pack of " << target_data->name_plural;
        } else {
          death_reason << "a " << target_data->name;
        }
        death_reason << " while hunting";
        kill_citizens(CIT_PEASANT, result.attackers_dead, DEATH_HUNTING,
                      death_reason.str());
      } // if (result.attackers_dead > 0)
    } // if (target != ANIMAL_NULL)
  } // while (hunting_points > 0)

// We get a warning about a lack of livestock space even if hunting messages are
// off.
  if (!animals_both.empty()) {
    add_message(MESSAGE_MAJOR, "We are out of space for livestock!");
  }
// Now, add a message about what we killed/caught.
  if (show_hunting_messages) {
    std::stringstream ss_message;

    if (animals_killed.empty() && animals_caught.empty() &&
        animals_both.empty()) {
      ss_message << "No animals killed or caught.";
    } else {
      ss_message << "Hunt result: ";
    }

    if (!animals_killed.empty()) {
      ss_message << capitalize( list_animals(animals_killed, "killed.") );
    }

    if (!animals_caught.empty()) {
      if (!animals_killed.empty()) {
        ss_message << "  ";
      }
      ss_message << capitalize( list_animals(animals_caught, "captured."));
    }

    if (!animals_both.empty()) {
      add_message(MESSAGE_MAJOR, "We are out of space for livestock!");
      if (!animals_killed.empty() || !animals_caught.empty()) {
        ss_message << "  ";
      }
      ss_message << capitalize( list_animals(animals_both,
                                             "captured, but had to be killed.")
                              );
    }

    std::string message = capitalize( trim( ss_message.str() ) );
    add_message(MESSAGE_MINOR, message);

  } // if (show_hunting_messages)

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
// Add the food gained to hunt_record_food, if we're tracking
    if (hunt_record_days >= 0) {
      hunt_record_food += amount * animal_dat->food_killed;
    }
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

int Player_city::get_livestock_total()
{
  int ret = 0;
  for (std::map<Animal,int>::iterator it = livestock.begin();
       it != livestock.end();
       it++) {
    Animal_datum* ani_dat = Animal_data[it->first];
    ret += it->second * ani_dat->size;
  }
  return ret;
}

int Player_city::get_livestock_capacity()
{
  int ret = 0;
  for (int i = 0; i < areas.size(); i++) {
    if (areas[i].is_open()) {
      ret += areas[i].building.livestock_space();
    }
  }
  return ret;
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

bool Player_city::meets_achievement(City_achievement achievement)
{
  int a = achievement.value_a, b = achievement.value_b;
  switch (achievement.type) {

    case CITY_ACHIEVE_NULL:
      return true;  // Not a real condition!

    case CITY_ACHIEVE_POP:
      return (population[a].count >= b);

    case CITY_ACHIEVE_ORES:
      return (get_mineral_amount(MINERAL_TIN   ) > 0 ||
              get_mineral_amount(MINERAL_COPPER) > 0 ||
              get_mineral_amount(MINERAL_IRON  ) > 0   );

    default:
      debugmsg("No code for City_achievement type %d", achievement.type);
      return false;
  }

  debugmsg("Escaped Player_city::meets_achievement switch.");
  return false;
}

nc_color message_type_color(Message_type type)
{
  switch (type) {
    case MESSAGE_NULL:    return c_dkgray;
    case MESSAGE_MINOR:   return c_ltgray;
    case MESSAGE_MAJOR:   return c_yellow;
    case MESSAGE_URGENT:  return c_ltred;
    case MESSAGE_UNLOCK:  return c_ltgreen;
    case MESSAGE_MAX:     return c_dkgray;
    default:              return c_magenta;
  }
  return c_magenta;
}

