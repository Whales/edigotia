#include "city.h"
#include "cuss.h"
#include "window.h"
#include "building.h"
#include "stringfunc.h"
#include "geometry.h"
#include "rng.h"
#include <sstream>
#include <vector>
#include <map>

Citizens::Citizens()
{
  count = 0;
  employed = 0;
  wealth = 0;
}

Citizens::~Citizens()
{
}

int Citizens::get_unemployed()
{
  if (employed >= count) {
    return 0;
  }
  return count - employed;
}

int Citizens::get_income()
{
// Obviously, don't include income from government jobs (i.e. areas/buildings)
  int ret = 0;

// Idle citizens do produce income though!
  int idle = get_unemployed();
  ret += (idle * citizen_idle_income(type)) / 100;

// TODO: Other sources of income?

  return ret;
}
  

City::City()
{
  for (int i = 0; i < CIT_MAX; i++) {
    population[i].type = Citizen_type(i);
  }

  population[CIT_PEASANT].count = 100;

  for (int i = 0; i < BUILD_MAX; i++) {
    open_buildings[i]   = 0;
    closed_buildings[i] = 0;
  }

  for (int i = 0; i < RES_MAX; i++) {
    resources[i] = 0;
  }
  for (int i = 0; i < MINERAL_MAX; i++) {
    minerals[i] = 0;
  }

  resources[RES_GOLD]  = 5000;
  resources[RES_FOOD]  = 5000;
  resources[RES_WOOD]  =  100;
  resources[RES_STONE] =  100;

  radius = 1;
  labor_pool = 0;
}

City::~City()
{
}

bool City::place_keep()
{
  Window w_map(0, 0, 40, 24);
  cuss::interface i_map;
  if (!i_map.load_from_file("cuss/city_map.cuss")) {
    return false;
  }

  i_map.set_data("text_info", "Start town here?\n<c=magenta>Y/N<c=/>");

  do {
    display_map(&w_map, &i_map);
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

void City::draw_map(cuss::element* e_draw, Point sel, bool radius_limited)
{
  if (!e_draw) {
    debugmsg("City::draw_map() called with NULL drawing area.");
    return;
  }

  std::map<Point,glyph,Pointcomp> drawing;

// Draw any constructed areas
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

void City::display_map(Window* w, cuss::interface* i_map, bool interactive,
                       bool radius_limited)
{
  bool owns_window = false, owns_interface = false;
  if (!i_map) {
    owns_interface = true;
    i_map = new cuss::interface;
    if (!i_map->load_from_file("cuss/city_map.cuss")) {
      delete i_map;
      return;
    }
  }
  if (!w) {
    owns_window = true;
    int xdim, ydim;
    get_screen_dims(xdim, ydim);
    w = new Window(0, 0, xdim, ydim);
  }

// Draw the map
  for (int x = 0; x < CITY_MAP_SIZE; x++) {
    for (int y = 0; y < CITY_MAP_SIZE; y++) {
      glyph gl = map.get_glyph(x, y);
      if (radius_limited && !inside_radius(x, y)) {
        gl.fg = c_dkgray;
      }
      i_map->set_data("draw_map", gl, x, y);
    }
  }

// Draw any constructed areas
  for (int i = 0; i < areas.size(); i++) {
    Area* area = &(areas[i]);
    Area_datum* areadata = Area_data[area->type];
    i_map->set_data("draw_map", areadata->symbol, area->pos.x, area->pos.y);
  }

// Draw any enqueued areas
  for (int i = 0; i < area_queue.size(); i++) {
    Area* area = &(area_queue[i]);
    Area_datum* areadata = Area_data[area->type];
    glyph gl = areadata->symbol;
    gl.bg = c_blue;
    i_map->set_data("draw_map", gl, area->pos.x, area->pos.y);
  }

  i_map->draw(w);
  w->refresh();
  if (!interactive) {
    if (owns_window) {
      delete w;
    }
    if (owns_interface) {
      delete i_map;
    }
    return;
  }

// Interactive part
  bool done = false;
  Area_type building = AREA_NULL;
  Point pos(CITY_MAP_SIZE / 2, CITY_MAP_SIZE / 2);

  while (!done) {
// Set data depending on the building type
    if (building == AREA_NULL) {
      i_map->clear_data("text_info");
    } else {
      i_map->set_data( "text_info", map.get_resource_info(pos) );
    }
    for (int x = 0; x < CITY_MAP_SIZE; x++) {
      for (int y = 0; y < CITY_MAP_SIZE; y++) {
        glyph gl = map.get_glyph(x, y);
        if (radius_limited && !inside_radius(x, y)) {
          gl.fg = c_dkgray;
        }
        if (x == pos.x && y == pos.y) {
          gl = gl.hilite(c_cyan);
        }
        i_map->set_data("draw_map", gl, x, y);
      }
    }

// Draw any constructed areas
    for (int i = 0; i < areas.size(); i++) {
      Area* area = &(areas[i]);
      Area_datum* areadata = Area_data[area->type];
      i_map->set_data("draw_map", areadata->symbol, area->pos.x, area->pos.y);
    }
// Draw any enqueued areas
    for (int i = 0; i < area_queue.size(); i++) {
      Area* area = &(area_queue[i]);
      Area_datum* areadata = Area_data[area->type];
      glyph gl = areadata->symbol;
      gl.bg = c_blue;
      i_map->set_data("draw_map", gl, area->pos.x, area->pos.y);
    }

    i_map->draw(w);
    w->refresh();
    long ch = getch();
    switch (ch) {
      case 'y':
      case '7':
        pos.x--;
        pos.y--;
        break;
      case 'k':
      case '8':
      case KEY_UP:
        pos.y--;
        break;
      case 'u':
      case '9':
        pos.x++;
        pos.y--;
        break;
      case 'h':
      case '4':
      case KEY_LEFT:
        pos.x--;
        break;
      case 'l':
      case '6':
      case KEY_RIGHT:
        pos.x++;
        break;
      case 'b':
      case '1':
        pos.x--;
        pos.y++;
        break;
      case 'j':
      case '2':
      case KEY_DOWN:
        pos.y++;
        break;
      case 'n':
      case '3':
        pos.x++;
        pos.y++;
        break;

      case 'a':
      case 'A': {
        std::stringstream area_options;
        for (int i = AREA_NULL + 1; i < AREA_MAX; i++) {
          area_options << "<c=magenta>" << i << "<c=/>: " <<
                          Area_data[i]->name << std::endl;
        }
        area_options << "<c=magenta>Q<c=/>: Cancel";
        i_map->set_data("text_info", area_options.str());
        i_map->draw(w);
        w->refresh();
        long area_ch = input();
        if (area_ch >= '1' && area_ch <= '1' + AREA_MAX - 2) {
          building = Area_type( area_ch - '1' + 1 );
        } else {
          building = AREA_NULL;
        }
      } break;

      case '\n':
        if (building != AREA_NULL) {
          add_area_to_queue(building, pos);
// Add building
        }
        break;

      case KEY_ESC:
      case 'q':
      case 'Q':
        done = true;
        break;
    }
    if (pos.x < 0) {
      pos.x = 0;
    }
    if (pos.y < 0) {
      pos.y = 0;
    }
    if (pos.x >= CITY_MAP_SIZE) {
      pos.x = CITY_MAP_SIZE - 1;
    }
    if (pos.y >= CITY_MAP_SIZE) {
      pos.y = CITY_MAP_SIZE - 1;
    }
  }
  if (owns_window) {
    delete w;
  }
  if (owns_interface) {
    delete i_map;
  }
}

void City::do_turn()
{
// Import resources.
  for (int i = 1; i < RES_MAX; i++) {
    Resource import = Resource(i);
    resources[import] += get_import(import);
  }

// Produce / eat food.
  resources[RES_FOOD] += get_food_production();
  int food_consumed = get_food_consumption();
  if (resources[RES_FOOD] >= food_consumed) {
    resources[RES_FOOD] -= food_consumed;
  } else {
// The upper classes get to eat first.
// TODO: Allow a law which changes this?
    for (int i = CIT_MAX - 1; i > CIT_NULL; i--) {
      Citizen_type cit_type = Citizen_type(i);
      int type_consumption = get_food_consumption(cit_type);
      if (resources[RES_FOOD] >= type_consumption)  {
        resources[RES_FOOD] -= type_consumption;
      } else {
        //int food_deficit = type_consumption - resources[RES_FOOD];
        resources[RES_FOOD] = 0;
// TODO: Starvation!
      }
    }
  }

// Produce minerals and wood.
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
// TODO: Add an alert for the player that we've exhausted this source of mineral
                minerals[min_mined.type] += tile_min->amount;
                tile->minerals.erase( tile->minerals.begin() + m );
// Fire any workers associated with that mineral.
// TODO: Don't hardcode CIT_PEASANT, even though it's a perfectly OK assumption
                fire_citizens(CIT_PEASANT, workers, mine_building);
                mine_building->minerals_mined.erase(
                  mine_building->minerals_mined.begin() + n
                );
                n--;
              } else {
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
      if (tile->wood < wood_produced) {
// TODO: Add a message alerting the player that the wood is exhausted.
        resources[RES_WOOD] += tile->wood;
        tile->wood = 0;
        tile->clear_wood();
        fire_citizens(CIT_PEASANT, sawmill_bldg->workers, sawmill_bldg);
        areas[i].open = false;
      } else {
        resources[RES_WOOD] += wood_produced;
        tile->wood -= wood_produced;
      }
    } // if (areas[i].produces_resource(RES_LOGGING))
  }

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
// TODO: Announce mineral discovery!
        popup("Discovered %s!", Mineral_data[min_amt->type]->name.c_str());
        min_amt->amount = 0;
      }
    }
  }
        

// Advance progress on the first area in our queue.
  if (!area_queue.empty()) {
    Area* area_to_build = &(area_queue[0]);
    area_to_build->construction_left--;
    if (area_to_build->construction_left <= 0) {
      add_open_area(area_queue[0]);
      area_queue.erase( area_queue.begin() );
    }
  }
}

Area_queue_status City::add_area_to_queue(Area_type type, Point location)
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

Area_queue_status City::add_area_to_queue(Area area)
{
  area.make_queued();  // Sets up construction_left.
  area_queue.push_back(area);
  return AREA_QUEUE_OK;
}

void City::add_open_area(Area area)
{
// Set it as open
  area.open = true;
// Figure out how many crops per field we get
  Building_datum* build_dat = area.get_building_datum();
  if (!build_dat) {
    debugmsg("NULL Building_data* in City::open_area (%s).",
             area.get_name().c_str());
  }

// Farms are set up specially.
// Check if this area produces RES_FARMING.
  int farming = 0;
  for (int i = 0; farming == 0 && i < build_dat->production.size(); i++) {
    if (build_dat->production[i].type == RES_FARMING) {
      farming = build_dat->production[i].amount;
    }
  }
  if (farming > 0) {
    Map_tile* tile_here = map.get_tile(area.pos);
    Building* farm_bld = &(area.building);
    farming = (farming * tile_here->get_farmability()) / 100;
// TODO: Further modify farming based on racial ability.
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

  areas.push_back( area );
}

bool City::expend_resource(Resource res, int amount)
{
  std::vector<Resource_amount> res_vec;
  res_vec.push_back( Resource_amount(res, amount) );
  return expend_resources(res_vec);
}

bool City::expend_resources(std::vector<Resource_amount> res_used)
{
// First, check if we have enough
  for (int i = 0; i < res_used.size(); i++) {
    Resource res = res_used[i].type;
    if (get_resource_amount(res) < res_used[i].amount) {
      return false;
    }
  }
// Now, actually consume them
  for (int i = 0; i < res_used.size(); i++) {
    Resource res = res_used[i].type;
    resources[res] -= res_used[i].amount;
    if (res == RES_GOLD && resources[res] < 0) {
      minerals[MINERAL_GOLD] += resources[res];
      resources[res] = 0;
    } else if (res == RES_STONE && resources[res] < 0) {
      minerals[MINERAL_STONE] += resources[res];
      resources[res] = 0;
    }
  }
  return true;
}

bool City::expend_resources(std::map<Resource,int> res_used)
{
  std::map<Resource,int>::iterator it;
// First, check if we have enough
  for (it = res_used.begin(); it != res_used.end(); it++) {
    Resource res = it->first;
    if (get_resource_amount(res) < it->second) {
      return false;
    }
  }
// Now, actually consume them
  for (it = res_used.begin(); it != res_used.end(); it++) {
    Resource res = it->first;
    resources[res] -= it->second;
    if (res == RES_GOLD && resources[res] < 0) {
      minerals[MINERAL_GOLD] += resources[res];
      resources[res] = 0;
    } else if (res == RES_STONE && resources[res] < 0) {
      minerals[MINERAL_STONE] += resources[res];
      resources[res] = 0;
    }
  }
  return true;
}

bool City::employ_citizens(Citizen_type type, int amount, Building* job_site)
{
// Several checks to ensure we can make this assignment
  if (!job_site) {
    return false;
  }
  if (type == CIT_NULL) {
    return false; // Gotta be a real class.
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

bool City::fire_citizens(Citizen_type type, int amount, Building* job_site)
{
// Several checks to ensure we can make this assignment
  if (!job_site) {
    return false;
  }
  if (type == CIT_NULL) {
    return false; // Gotta be a real class.
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

bool City::inside_radius(int x, int y)
{
  return inside_radius( Point(x, y) );
}

bool City::inside_radius(Point p)
{
  Point center(CITY_MAP_SIZE / 2, CITY_MAP_SIZE / 2);

  return rl_dist(center, p) <= radius;
}

Area* City::area_at(int x, int y)
{
  return area_at( Point(x, y) );
}

Area* City::area_at(Point p)
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

std::string City::get_map_info(Point p)
{
  std::stringstream ret;
  ret << map.get_info(p);
  Area* area = area_at(p);
  if (area) {
    ret << std::endl;
    ret << area->get_name();
    if (area->construction_left > 0) {
      ret << " (<c=brown>Under Construction<c=/>)";
    } else if (!area->open) {
      ret << " (<c=red>Closed<c=/>)";
    }
  }

  return ret.str();
}

// type defaults to CIT_NULL
int City::get_total_population(Citizen_type type)
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
int City::get_unemployed_citizens(Citizen_type type)
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
int City::get_total_housing(Citizen_type type)
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

int City::get_military_count()
{
  int ret = 0;
  for (int i = 0; i < units_stationed.size(); i++) {
    ret += units_stationed[i].count;
  }
  return ret;
}

int City::get_military_supported()
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

std::vector<Building*> City::get_all_buildings()
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

int City::get_number_of_buildings(Building_type type)
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

int City::get_number_of_areas(Area_type type)
{
  int ret = 0;
  for (int i = 0; i < areas.size(); i++) {
    if (type == AREA_NULL || areas[i].type == type) {
      ret++;
    }
  }
  return ret;
}

int City::get_total_maintenance()
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

int City::get_fields_worked()
{
  int ret = 0;
  for (int i = 0; i < areas.size(); i++) {
    if (areas[i].open && areas[i].type == AREA_FARM) {
      ret += areas[i].building.workers;
    }
  }
  return ret;
}

int City::get_empty_fields()
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

int City::get_shafts_worked()
{
  int ret = 0;
  for (int i = 0; i < areas.size(); i++) {
    if (areas[i].open && areas[i].type == AREA_MINE) {
      ret += areas[i].building.workers;
    }
  }
  return ret;
}

int City::get_free_shafts()
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

int City::get_resource_amount(Resource res)
{
  if (res == RES_GOLD) {
    return resources[res] + minerals[MINERAL_GOLD];
  }
  if (res == RES_STONE) {
    return resources[res] + minerals[MINERAL_STONE];
  }
  return resources[res];
}

int City::get_mineral_amount(Mineral min)
{
  if (min == MINERAL_GOLD) {
    return minerals[min] + resources[RES_GOLD];
  }
  if (min == MINERAL_STONE) {
    return minerals[min] + resources[RES_STONE];
  }
  return minerals[min];
}

// type defaults to CIT_NULL
int City::get_total_wages(Citizen_type type)
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
int City::get_military_expense()
{
  int ret = 0;
  for (int i = 0; i < units_stationed.size(); i++) {
    ret += units_stationed[i].count / 5;
  }
  return ret;
}

// type defaults to CIT_NULL
int City::get_taxes(Citizen_type type)
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
int City::get_corruption_percentage()
{
  return 10;
}

int City::get_corruption_amount()
{
  int percentage = get_corruption_percentage();
  int income = get_taxes() + get_import(RES_GOLD) +
               get_amount_mined(MINERAL_GOLD);
  int lost = income * percentage;
  lost /= 100;  // Since percentage is reported as an int from 0 to 100.
  return lost;
}

// type defaults to CIT_NULL
int City::get_food_consumption(Citizen_type type)
{
  if (type == CIT_NULL) {
    int ret = 0;
    for (int i = 0; i < CIT_MAX; i++) {
      int a = population[i].count * citizen_food_consumption( Citizen_type(i) );
      ret += a;
    }
    return ret;
  }
  return (population[type].count * citizen_food_consumption( type ));
}

int City::get_food_production()
{
  int ret = 0;
  std::vector<Crop_amount> crops_grown = get_crops_grown();
  for (int i = 0; i < crops_grown.size(); i++) {
    Crop_amount crop = crops_grown[i];
    ret += crop.amount * Crop_data[crop.type]->food;
  }
  return ret;
}

std::vector<Crop_amount> City::get_crops_grown()
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
int City::get_amount_mined(Mineral mineral)
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

std::map<Mineral,int> City::get_minerals_mined()
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
std::map<Mineral,int> City::get_minerals_used()
{
  std::map<Mineral,int> ret;
  return ret;
}

// TODO: This function.
int City::get_import(Resource res)
{
  return 0;
}

// TODO: This function.
int City::get_export(Resource res)
{
  return 0;
}
