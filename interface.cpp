#include "interface.h"
#include "window.h"
#include "cuss.h"
#include "city.h"
#include "stringfunc.h"
#include "keys.h"
#include "building.h"
#include <sstream>
#include <cstdarg> // For the variadic function below

Interface::Interface()
{
  cur_menu = MENU_NULL;
  cur_mode = IMODE_NULL;
  next_menu_posx = 2;
  sel = Point(4, 4);
  city_radius = true;
  current_area = AREA_NULL;
  game = NULL;
  city = NULL;
}

Interface::~Interface()
{
}

bool Interface::init(Game* G, City* C)
{
  if (!G) {
    if (!C) {
      debugmsg("Interface init'd with NULL game AND NULL city!");
    } else {
      debugmsg("Interface init'd with NULL game.");
    }
    return false;
  } else if (!C) {
    debugmsg("Interface init'd with NULL city.");
    return false;
  }

  game = G;
  city = C;

  if (!i_main.load_from_file("cuss/interface.cuss")) {
    debugmsg("Failed to load critical interface file cuss/interface.cuss!");
    return false;
  }

  w_main.init(0, 0, 80, 24);

// Put menus here
  add_menu(MENU_GAME, "Game",
"Save & Quit",
"Quit without saving",
"About",
0
);

  add_menu(MENU_MINISTERS, "Ministers",
"Finance",
"Happiness",
0
);

  add_menu(MENU_BUILDINGS, "Buildings",
"Status",
"Build",
0
);

  set_menu_str();

  return true;
}

void Interface::main_loop()
{
  sel = Point(4, 4);
  city_radius = true;

  i_main.set_data("text_menu_bar", menu_str);

  set_mode(IMODE_VIEW_MAP);

  int date_size = i_main.element_width("text_date");

  bool done = false;
  while (!done) {
    i_main.set_data("text_date", game->get_date_str(date_size));
    city->draw_map(i_main.find_by_name("draw_map"), sel, city_radius);
    i_main.set_data("num_population",   city->get_total_population());
    i_main.set_data("num_gold",         city->resources[RES_GOLD]);
    i_main.set_data("num_food",         city->resources[RES_FOOD]);
    i_main.set_data("num_wood",         city->resources[RES_WOOD]);
    i_main.set_data("num_stone",        city->resources[RES_STONE]);

    if (cur_mode == IMODE_VIEW_MAP && current_area != AREA_NULL) {
      display_area_stats(current_area);
    }

    i_main.draw(&w_main);
    w_main.refresh();
    long ch = input();

    restore_info_text();  // Undo temporary change to text_info

    if (ch == KEY_ESC) {
      set_menu(MENU_NULL);
      set_mode(IMODE_VIEW_MAP);

    } else if (ch == '!') {
      set_mode(IMODE_MENU);
      set_menu(MENU_NULL);

    } else {
      handle_key(ch);
    }

    if (game_state == GAME_QUIT) {
      done = true;
    }
  }
}

void Interface::handle_key(long ch)
{
//debugmsg("handle_key");
  if (ch >= '1' && ch <= '9') { // Accessing or using a menu!

/* We start counting at 1 because this is tied to the interface.  All menus and
 * items in menus start counting at 1, so we do too.
 */
    int menu_index = ch - '0';

    if (cur_menu == MENU_NULL) { // We're not in a menu - so open one
      set_menu( Menu_id( menu_index ) );
    } else {
      do_menu_action(cur_menu, menu_index);
      set_menu( MENU_NULL );
    }

  } else {  // Not a menu; thus, the action taken depends on our mode.
//debugmsg("mode %d", cur_mode);

    switch (cur_mode) {
      case IMODE_VIEW_MAP: {
        Point p = input_direction(ch);
//debugmsg("Mapviewing; %s", p.str().c_str());
        if (p.x != -2 && (p.x != 0 || p.y != 0)) {
          sel += p;
        } else if (ch == '\n') {
          if (current_area != AREA_NULL) {
            enqueue_area();
          } else {
            popup( city->map.get_info(sel).c_str() );
          }
        } else if (ch == 'q' || ch == 'Q') {
          current_area = AREA_NULL;
          set_mode(IMODE_VIEW_MAP);
        } else if (ch == 'r' || ch == 'R') {
          city_radius = !city_radius;
        } else if (ch == 'a' || ch == 'A') {
          current_area = pick_area();
          display_area_stats(current_area);
          set_mode(IMODE_VIEW_MAP);
          Building_datum* build = get_building_for(current_area);
          if (current_area != AREA_NULL && build) {
            i_main.set_data("text_info", build->get_short_description());
          }
        } else if (ch == '.') {
          game->advance_time(1, city);
        } else if (ch == '>') {
          game->advance_time(7, city);
        }
      } break;
          
// TODO: Fill this in with all keybindings for all modes.
    }

  }
}

void Interface::set_mode(Interface_mode mode)
{
  cur_mode = mode;
  i_main.clear_data("text_info"); // TODO: No?
  //i_main.clear_data("text_info");
  switch (mode) {

    case IMODE_NULL:
    case IMODE_MENU:
      i_main.clear_data("text_commands");
      break;

    case IMODE_VIEW_MAP: {
      std::stringstream commands;
      commands << "Use movement keys to scroll." << std::endl;
      if (current_area != AREA_NULL) {
        commands << "<c=pink>Enter<c=/>: Place " <<
                    Area_data[current_area]->name << std::endl <<
                    "<c=pink>Q<c=/>: Cancel " <<
                    Area_data[current_area]->name << " placement" << std::endl;
      } else {
        commands << "<c=pink>Enter<c=/>: Get info on tile" << std::endl;
      }
      commands << "<c=pink>R<c=/>: Toggle control radius" << std::endl;
      commands << "<c=pink>A<c=/>: Build ";
      if (current_area != AREA_NULL) {
        commands << "a different ";
      }
      commands << "area" << std::endl;
      commands << "<c=pink>C<c=/>: Close area" << std::endl;
      commands << "<c=pink>D<c=/>: Destroy area" << std::endl;

      i_main.set_data("text_commands", commands.str());

    } break;

  }

}

void Interface::set_menu(Menu_id item)
{
  std::string menu_name;
  int posx = -1;
// If we're setting the currently-open menu, instead just close it.
  if (item == cur_menu) {
    item = MENU_NULL;
  }
// Remove any currently-open menu.
  if (cur_menu != MENU_NULL) {
    set_mode(IMODE_MENU);
    get_menu_info(cur_menu, menu_name, posx);
    i_main.erase_element(menu_name);
    i_main.erase_element("menu_border");
  }
// Add in the new menu.
  cur_menu = item;
  if (item == MENU_NULL) {  // Nothing else to do.
    return;
  }

  get_menu_info(item, menu_name, posx);
  std::vector<std::string> menu_items = get_menu_options(item);
  int sizex = 0, sizey = menu_items.size();
  for (int i = 0; i < menu_items.size(); i++) {
    int ln = tagless_length(menu_items[i]);
    if (ln > sizex) {
      sizex = ln;
    }
  }

  sizex++;

  i_main.add_element(cuss::ELE_LIST, menu_name, posx, 1, sizex, sizey);
  i_main.add_element(cuss::ELE_DRAWING, "menu_border",
                     posx - 1, 1, sizex + 2, sizey + 1);

  i_main.set_selectable(menu_name, false);
  i_main.add_data(menu_name, menu_items);

// Draw the border... ick, manual drawing, oh well.
  glyph line_ns(LINE_XOXO, c_white, c_black);
  glyph line_ew(LINE_OXOX, c_white, c_black);
  for (int y = 0; y < sizey; y++) {
    i_main.set_data("menu_border", line_ns,         0, y);
    i_main.set_data("menu_border", line_ns, sizex + 1, y);
  }
  for (int x = 1; x < sizex + 1; x++) {
    i_main.set_data("menu_border", line_ew, x, sizey);
  }
// And the corners.
  glyph line_ne(LINE_XXOO, c_white, c_black);
  glyph line_nw(LINE_XOOX, c_white, c_black);
  i_main.set_data("menu_border", line_ne, 0, sizey);
  i_main.set_data("menu_border", line_nw, sizex + 1, sizey);
}

void Interface::do_menu_action(Menu_id menu, int index)
{
// By default, we fall back into normal map mode.  This is suitable most of the
// time, but if not you can override it below.
  set_mode(IMODE_VIEW_MAP);
  switch (menu) {
    case MENU_GAME:
      switch (index) {
        case 1: // Save and quit
          popup("Can't save yet!");
          break;
        case 2: // Just quit
          game_state = GAME_QUIT;
          break;
        case 3: // About
          popup("Edigotia is (c) 2014 Whales");
          break;
      }
      break;

    case MENU_MINISTERS:
      break;

    case MENU_BUILDINGS:
      switch (index) {
        case 1: // Build area
          //current_area = pick_area();
          set_mode(IMODE_VIEW_MAP);
          break;
        case 2: // Build building
          break;
      }
      break;

// TODO: Other menus.

  }
}

void Interface::set_temp_info(std::string text)
{
  original_info_text = i_main.get_str("text_info");
  i_main.set_data("text_info", text);
}

void Interface::restore_info_text()
{
  if (!original_info_text.empty()) {
    i_main.set_data("text_info", original_info_text);
    original_info_text = "";
  }
}

void Interface::display_area_stats(Area_type type)
{
  i_main.clear_data("text_options");

  std::stringstream stats; // We'll set text_options to this

  Building_datum* build_dat = Area_data[type]->get_building_datum();
  std::string area_name = Area_data[type]->name;

  Map_tile* tile = city->map.get_tile(sel);

  switch (type) {

    case AREA_HOVELS:
    case AREA_HOUSES:
    case AREA_MANOR:
    case AREA_KEEP:
      for (int i = 0; i < build_dat->housing.size(); i++) {
        Citizen_type cit_type = build_dat->housing[i].type;
        std::string cit_name = citizen_type_name(cit_type);
        cit_name = capitalize(cit_name);
        std::string plural_name = citizen_type_name(cit_type, true);
        int pop     = city->get_total_population(cit_type);
        int housing = city->get_total_housing(cit_type);
  
        stats << "<c=white>" << cit_name << " population: " << pop << "<c=/>" <<
                 std::endl;
        stats << "<c=white>" << cit_name << " housing:    ";
        if (pop == housing) {
          stats << "<c=yellow>";
        } else if (pop > housing) {
          stats << "<c=ltred>";
        }
        stats << housing << "<c=/>" << std::endl;
  
        stats << "<c=white>" << capitalize(area_name) << " provide";
        if (!build_dat->plural) {
          stats << "s";
        }
        stats << " housing for " << build_dat->housing[i].amount << " " <<
                 plural_name << "<c=/>" << std::endl << std::endl;
      }
      break;

    case AREA_FARM:
      stats << "Food consumed each day: " << city->get_food_consumption() <<
               std::endl;
      stats << "Food produced each day: " << city->get_food_production() <<
               std::endl;
      stats << tile->get_terrain_name() << " farmability: " <<
               tile->get_farmability() << "%%%%" << std::endl;
      stats << "Crops here: " << tile->get_crop_info();
      break;

    case AREA_QUARRY:
      break;

    case AREA_MINE:
      break;  // TODO: Info on available minerals?

    case AREA_BARRACKS:
      stats << "Number of soldiers: " << city->get_military_count() <<
               std::endl;
      stats << "Soldiers supported: " << city->get_military_supported();
      break;
  }

  i_main.set_data("text_options", stats.str());
}

void Interface::enqueue_area()
{
  if (current_area == AREA_NULL) {
    return;
  }
  Building_datum* build = get_building_for(current_area);

  if (city->expend_resources(build->build_costs)) {
    Area_queue_status stat = city->add_area_to_queue(current_area, sel);
    if (stat != AREA_QUEUE_OK) {
      std::stringstream fail;
      fail << "<c=ltred>";
      switch (stat) {
        case AREA_QUEUE_OUTSIDE_RADIUS:
          fail << "That location is outside your control.";
          break;
        case AREA_QUEUE_BAD_TERRAIN:
          fail << "You cannot build that there.";
          break;
        case AREA_QUEUE_OCCUPIED:
          fail << "There is already an area there.";
          break;
      }
      fail << "<c=/>";
      set_temp_info(fail.str());
    }
  } else {
    current_area = AREA_NULL;
    set_mode(IMODE_VIEW_MAP);
    set_temp_info("<c=ltred>You do not have the resources to build that!<c=/>");
  }
}

void Interface::minister_finance()
{
}

Area_type Interface::pick_area()
{
  std::vector<std::string> area_options;
  i_main.clear_data("text_info");
  i_main.clear_data("text_options");
  for (int i = 1; i < AREA_MAX; i++) {
    std::stringstream option;
    option << "<c=magenta>";
    if (i <= 9) {
      option << i;
    } else {
      option << char(i - 10 + 'A');
    }
    option << "<c=/>: " << Area_data[i]->name << std::endl;
    i_main.add_data("text_options", option.str());
  }

  i_main.draw(&w_main);
  w_main.refresh();

  while (true) {
    long ch = input();
    if (ch == 'q' || ch == 'Q' || ch == KEY_ESC || ch == '0') {
      i_main.clear_data("text_options");
      return AREA_NULL;
    }
    int sel = -1;
    if (ch >= '1' && ch <= '9') {
      sel = ch - '0';
    } else if (ch >= 'a' && ch <= 'z') {
      sel = ch - 'a' + 10;
    } else if (ch >= 'A' && ch <= 'Z') {
      sel = ch - 'A' + 10;
    }
    if (sel >= 1 && sel < AREA_MAX) {
      i_main.clear_data("text_options");
      return Area_type(sel);
    }
  }
}

void Interface::get_menu_info(Menu_id item, std::string& name, int& posx)
{
  name = menus[item - 1].name;
  name = remove_color_tags(name);
  name = name.substr(3);  // Remove "1: "
  posx = menus[item - 1].posx;
}

std::vector<std::string> Interface::get_menu_options(Menu_id item)
{
  std::vector<std::string> ret;

  for (int i = 0; i < menus[item - 1].items.size(); i++) {
    std::stringstream ss_name;
    ss_name << "<c=pink>" << i + 1 << "<c=/>: " << menus[item - 1].items[i];
    ret.push_back( ss_name.str() );
  }

  return ret;
}

/*
  switch (item) {
    case MENU_MINISTERS:
      ret.push_back( menuify("Finance")   );
      ret.push_back( menuify("Public")    );
      ret.push_back( menuify("Trade")     );
      ret.push_back( menuify("Diplomacy") );
      ret.push_back( menuify("Military")  );
      break;
  }

  return ret;
}
*/

bool Interface::add_menu(Menu_id id, std::string name, ...)
{
  int length = tagless_length(name) + 3;  // +3 for "1: "
/*
  if (posx + length > 62) {
    debugmsg("Tried to add menu '%s' at posx %d, but that name is too long!",
             name.c_str(), posx);
    return false;
  }
*/
  Menu tmp_menu;
  std::stringstream ss_name;
  ss_name << "<c=pink,blue>" << int(id) << "<c=white,blue>: " << name;
  tmp_menu.name = ss_name.str();
  tmp_menu.posx = next_menu_posx;
  next_menu_posx += length + 2;
  va_list ap;
  va_start(ap, name);
  char *tmpstr;
  while ( (tmpstr = (char*)(va_arg(ap, char*))) ) {
    tmp_menu.items.push_back( std::string(tmpstr) );
  }
  va_end(ap);

  menus.push_back(tmp_menu);
  return true;
}

void Interface::set_menu_str()
{
  std::stringstream ss_menus;
  ss_menus << "<c=white,blue>";
  for (int i = 0; i < menus.size(); i++) {
    ss_menus << "  " << menus[i].name;
  }
  ss_menus << "<c=/>";
  menu_str = ss_menus.str();
  int width = i_main.element_width("text_menu_bar");
  if (width == -1) {
    debugmsg("text_menu_bar not found in i_main!");
    std::vector<std::string> ele_names = i_main.element_names();
    for (int i = 0; i < ele_names.size(); i++) {
      debugmsg("'%s'", ele_names[i].c_str());
    }
    return;
  }
  if (remove_color_tags(menu_str).length() < width) {
    menu_str += "<c=blue,blue>";
    while (remove_color_tags(menu_str).length() < width) {
      menu_str += 'x';
    }
  }
}

std::string menuify(std::string name)
{
  if (name.empty()) {
    return std::string();
  }
  std::stringstream ret;
  ret << "<c=ltred>" << name[0] << "<c=/>" << name.substr(1);
  return ret.str();
}
