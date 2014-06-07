#include "city.h"
#include "cuss.h"
#include "window.h"
#include "building.h"
#include "stringfunc.h"
#include <sstream>
#include <vector>

City::City()
{
  population[CIT_NULL]     =   0;
  population[CIT_SLAVE]    =   0;
  population[CIT_PEASANT]  = 100;
  population[CIT_MERCHANT] =   0;
  population[CIT_BURGHER]  =   0;
  population[CIT_NOBLE]    =   0;

  for (int i = 0; i < BUILD_MAX; i++) {
    open_buildings[i]   = 0;
    closed_buildings[i] = 0;
  }

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
    draw_map(&w_map, &i_map);
    i_map.set_data("draw_map", glyph('@', c_yellow, c_black),
                   CITY_MAP_SIZE / 2, CITY_MAP_SIZE / 2);
    i_map.draw(&w_map);
    w_map.refresh();
    long ch = getch();
    if (ch == 'Y' || ch == 'y') {
      Area keep;
      keep.type = AREA_KEEP;
      keep.pos = Point(CITY_MAP_SIZE / 2, CITY_MAP_SIZE / 2);
      areas.push_back(keep);
      return true;
    } else if (ch == 'n' || ch == 'N') {
      return false;
    }
  } while (true);
  return false;
}

void City::interface_buildings()
{
  cuss::interface i_buildings;
  Window w_buildings(0, 0, 80, 24);

  if (!i_buildings.load_from_file("cuss/buildings.cuss")) {
    return;
  }

  std::vector<std::string> building_names, building_count;
  std::vector<Building_type> building_types; // To key name list to types

  for (int i = 0; i < BUILD_MAX; i++) {
    if (open_buildings[i] > 0) {
      building_names.push_back( Building_data[i]->name  );
      building_count.push_back( itos(open_buildings[i]) );
      building_types.push_back( Building_type(i)        );
    }
  }

  i_buildings.set_data("list_buildings", building_names);
  i_buildings.set_data("list_count",     building_count);
  i_buildings.select  ("list_buildings");

  bool done = false;
  while (!done) {
    i_buildings.draw(&w_buildings);
    w_buildings.refresh();

    int index = i_buildings.get_int("list_buildings");
    Building_type type = building_types[index];

    i_buildings.set_data("num_maintenance", Building_data[type]->upkeep);

    long ch = input();
    if (ch == 'q' || ch == 'Q') {
      done = true;
    } else {
      i_buildings.handle_keypress(ch);
    }
  }
}

void City::draw_map(Window* w, cuss::interface* i_map, bool interactive,
                    bool radius_limited)
{
  bool owns_window = false, owns_interface = false;
  Point center(CITY_MAP_SIZE / 2, CITY_MAP_SIZE / 2);
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
      Terrain_type ter = map.tiles[x][y];
      Terrain_datum* terdata = Terrain_data[ter];
      glyph gl = terdata->symbol;
      if (radius_limited && rl_dist( Point(x, y), center) > radius) {
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
      i_map->set_data( "text_info", map.get_resource_info(pos.x, pos.y) );
    }
    for (int x = 0; x < CITY_MAP_SIZE; x++) {
      for (int y = 0; y < CITY_MAP_SIZE; y++) {
        Terrain_type ter = map.tiles[x][y];
        Terrain_datum* terdata = Terrain_data[ter];
        glyph gl = terdata->symbol;
        if (radius_limited && rl_dist( Point(x, y), center) > radius) {
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
// First, deduct maintenance/wages for all areas
/* TODO:  This could be better.  It could be the case that while we can't afford
 *        these costs NOW, we WILL be able to afford them after collecting
 *        resources from areas & buildings.
 */
  for (int i = 0; i < areas.size(); i++) {
    if (areas[i].open) {
      if (!expend_resources( areas[i].get_maintenance() )) {
        areas[i].open = false;
      }
    }
  }
// Ditto for buildings (and ditto for the TODO)
  for (int i = 0; i < BUILD_MAX; i++) {
    int bd_num = open_buildings[i];
    for (int n = 0; n < bd_num; n++) {
      Building_datum* bd_data = Building_data[i];
      if (!expend_resources( bd->maintenance_cost )) {
        open_buildings[i]--;
        closed_buildings[i]++;
      }
    }
  }
        
}

void City::add_area_to_queue(Area_type type, Point location)
{
  Area tmp(type, location);
  add_area_to_queue(tmp);
}

void City::add_area_to_queue(Area area)
{
  area_queue.push_back(area);
}

bool City::expend_resources(std::map<Resource,int> resources)
{
  std::map<Resource,int>::iterator it;
// Check if we have enough
  for (it = resources.begin(); it != resources.end(); it++) {
    Resource res = it->first;
    if (resources[res] < it->second) {
      return false;
    }
  }
// Now actually use them
  for (it = resources.begin(); it != resources.end(); it++) {
    Resource res = it->first;
    resources[res] -= it->second;
  }
  return true;
}
