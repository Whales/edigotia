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

// Menu setup!  Put menus here.
  add_menu(MENU_GAME, "Game",
"Save & Quit",
"Quit without saving",
"About",
0
);

  add_menu(MENU_MINISTERS, "Ministers",
"Finance",
"Food",
"Morale",
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
      set_mode( IMODE_VIEW_MAP ); // Get out of menu mode
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
      switch (index) {
        case 1: // Finance minister
          minister_finance();
          break;
        case 2: // Food minister
          minister_food();
          break;
        case 3: // Morale minister
          minister_morale();
          break;
      }
      break;

    case MENU_BUILDINGS:
      switch (index) {
        case 1: // View buildings
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

void Interface::minister_food()
{
  Window w_food(0, 0, 80, 24);
  cuss::interface i_food;
  if (!i_food.load_from_file("cuss/food.cuss")) {
    return;
  }

  i_food.select("list_crops");

// These values and fields are static during the life of this interface.
  int num_farms     = city->get_number_of_buildings(BUILD_FARM);
  int food_stored   = city->get_resource_amount(RES_FOOD);
  int food_imported = city->get_import(RES_FOOD);
  int food_consumed = city->get_food_consumption();
  int food_exported = city->get_export(RES_FOOD);

  i_food.set_data("num_farms",         num_farms);
  if (num_farms == 0) {
    i_food.set_data("num_farms", c_red);
  }

  i_food.set_data("num_food_stored",   food_stored);
  if (food_stored == 0) {
    i_food.set_data("num_food_stored", c_red);
  } else {
    i_food.set_data("num_food_stored", c_ltcyan);
  }

  i_food.set_data("num_food_imported", food_imported);
  if (food_imported > 0) {
    i_food.set_data("num_food_imported", c_ltgreen);
  } else {
    i_food.set_data("num_food_imported", c_dkgray);
  }

  i_food.set_data("num_food_consumed", food_consumed);
  if (food_consumed > 0) {
    i_food.set_data("num_food_consumed", c_ltred);
  } else {
    i_food.set_data("num_food_consumed", c_dkgray);
  }

  i_food.set_data("num_food_exported", food_exported);
  if (food_exported > 0) {
    i_food.set_data("num_food_exported", c_ltred);
  } else {
    i_food.set_data("num_food_exported", c_dkgray);
  }

// Set up a list of farms.
  std::vector<std::string> farm_terrain, farm_output, farm_fields;
  std::vector<Area*> farms;
  for (int i = 0; i < city->areas.size(); i++) {
    if (city->areas[i].produces_resource(RES_FARMING)) {
      Point area_loc = city->areas[i].pos;
      Building* build = &(city->areas[i].building);
      std::stringstream output_ss;
      std::stringstream empty_fields_ss;
      int empty_fields = build->get_empty_fields();

      output_ss << build->field_output << "0%%%%";
      if (empty_fields > 0) {
        empty_fields_ss << "<c=ltgreen>" << empty_fields << "<c=/>";
      } else {
        empty_fields_ss << empty_fields;
      }
      farms.push_back( &(city->areas[i]) );
      farm_terrain.push_back( city->map.get_terrain_name(area_loc) );
      farm_output.push_back( output_ss.str() );
      farm_fields.push_back( empty_fields_ss.str() );
    }
  }

  i_food.ref_data("list_farms",        &farm_terrain);
  i_food.ref_data("list_farm_output",  &farm_output);
  i_food.ref_data("list_empty_fields", &farm_fields);

/* We also need to list crops grown at the currently-selected farm.
 * We always start by selecting farms[0]; later on, if we select a new farm,
 * we'll update the list.  We don't want to update the list on every cycle of
 * the loop because that'll clear list_crop_name and move the cursor in that
 * list back to 0, making it impossible to select any other crop.
 */
  int farm_index = 0;
  Area* cur_farm = NULL;
  if (!farms.empty()) {
    cur_farm = farms[ farm_index ];
    list_farm_crops(cur_farm, i_food);
  }

  i_food.select("list_farms");

  bool done = false;

  while (!done) {
// These values and fields may change before we exit this interface.
// So we set them inside the loop.
    int fields_worked = city->get_fields_worked();
    int fields_empty  = city->get_empty_fields();
    int food_grown    = city->get_food_production();

    int net_food = food_grown + food_imported - food_exported - food_consumed;

    int days_left = -1;
    if (net_food < 0) {
      days_left = food_stored / (0 - net_food);
    }

    i_food.set_data("num_fields_worked", fields_worked);
    if (fields_worked == 0 && num_farms > 0) {
      i_food.set_data("num_fields_worked", c_red);
    } else if (num_farms == 0) {
      i_food.set_data("num_fields_worked", c_dkgray);
    } else {
      i_food.set_data("num_fields_worked", c_ltgray);
    }

    i_food.set_data("num_fields_empty", fields_empty);
    if (fields_empty > 0 && num_farms > 0) {
      i_food.set_data("num_fields_empty", c_yellow);
    } else if (num_farms == 0) {
      i_food.set_data("num_fields_empty", c_dkgray);
    } else {
      i_food.set_data("num_fields_empty", c_ltgray);
    }
    
    i_food.set_data("num_food_grown", food_grown);
    if (food_grown > 0) {
      i_food.set_data("num_food_grown", c_ltgreen);
    } else {
      i_food.set_data("num_food_grown", c_dkgray);
    }

    i_food.set_data("num_net_food", net_food);
    if (net_food < 0) {
      i_food.set_data("num_net_food", c_ltred);
    } else if (net_food > 0) {
      i_food.set_data("num_net_food", c_ltgreen);
    } else {
      i_food.set_data("num_net_food", c_ltblue);
    }

    int free_peasants = city->get_unemployed_citizens(CIT_PEASANT);
    i_food.set_data("num_free_peasants", free_peasants);
    if (fields_empty == 0) {  // We don't care since all fields are used
      i_food.set_data("num_free_peasants", c_dkgray);
    } else {  // We care since we could utilize a field
      if (free_peasants == 0) {
        i_food.set_data("num_free_peasants", c_red);
      } else {
        i_food.set_data("num_free_peasants", c_ltgreen);
      }
    }

    std::stringstream days_left_ss;
    if (days_left == -1) { // We'll never run out of food at this rate!
      days_left_ss << "<c=dkgray>N/A<c=/>";
    } else {
      if (days_left < 10) {
        days_left_ss << "<c=ltred>";
      } else if (days_left < 25) {
        days_left_ss << "<c=red>";
      } else {
        days_left_ss << "<c=yellow>";
      }
      days_left_ss << days_left << "<c=/>";
    }
    i_food.set_data("text_days_left", days_left_ss.str());

// Check if we selected a new farm.
    int new_farm_index = i_food.get_int("list_farms");
    if (new_farm_index != farm_index && new_farm_index >= 0 &&
        new_farm_index < farms.size()) {
      farm_index = new_farm_index;
      cur_farm = farms[farm_index];
      list_farm_crops(cur_farm, i_food);
    }

    i_food.draw(&w_food);
    w_food.refresh();

    int crop_change = 0;
    long ch = input();
    switch (ch) {
      case KEY_ESC:
      case 'q':
      case 'Q':
        done = true;
        break;
      case KEY_LEFT:
      case 'h':
      case 'H':
      case '4':
      case '-':
        crop_change = -2;
// Intentionally fall through...
      case KEY_RIGHT:
      case 'l':
      case 'L':
      case '+': {
        bool did_it = false;  // Track if changes (& interface will need update)
        int crop_index = i_food.get_int("list_crop_name");
        crop_change++;  // If it was -2, it's now -1; otherwise, it's now +1
// These keys are only meaningful if list_crop_name is selected.
        if (i_food.selected()->name == "list_crop_name") {
          Building* farm_build = &(cur_farm->building);
          if (crop_index >= 0 && crop_index < farm_build->crops_grown.size()) {
            if (crop_change < 0) {
              crop_change = 0 - crop_change;  // Absolute value that bitch
              if (farm_build->crops_grown[crop_index].amount >= crop_change &&
                city->fire_citizens(CIT_PEASANT, crop_change, farm_build)) {
                farm_build->crops_grown[crop_index].amount -= crop_change;
                did_it = true;
              }
            } else if (crop_change > 0 &&
                       farm_build->get_empty_fields() >= crop_change &&
                       city->employ_citizens(CIT_PEASANT, crop_change,
                                             farm_build)) {
              farm_build->crops_grown[crop_index].amount += crop_change;
              did_it = true;
            }
          }
        }
        if (did_it) {
          list_farm_crops(cur_farm, i_food);
// Reset our position in the list to what it was previously!
          i_food.set_data("list_crop_name", crop_index);
// Fix the list of empty fields
          std::stringstream empty_fields_ss;
          Building* build = &(farms[farm_index]->building);
          int empty_fields = build->get_empty_fields();
          if (empty_fields > 0) {
            empty_fields_ss << "<c=ltgreen>" << empty_fields << "<c=/>";
          } else {
            empty_fields_ss << empty_fields;
          }
          farm_fields[farm_index] = empty_fields_ss.str();
        }
      } break;
          
          
      default:
        i_food.handle_action(ch);
        break;
    }
  }

}

void Interface::list_farm_crops(Area* cur_farm, cuss::interface& i_food)
{
// List crops grown at cur_farm
  std::vector<std::string> crop_names, crop_types, crop_food, crop_grown;
  Building* farm_build = &(cur_farm->building);
  for (int i = 0; i < farm_build->crops_grown.size(); i++) {
    Crop_amount cur_crop = farm_build->crops_grown[i];
    Crop_datum* crop_dat = Crop_data[cur_crop.type];
    int crop_food_amount = crop_dat->food * farm_build->field_output;

    crop_names.push_back(crop_dat->name);
    crop_types.push_back( crop_type_name( crop_dat->type ) );
    crop_food.push_back ( itos( crop_food_amount ) );
    crop_grown.push_back( itos( cur_crop.amount ) );
  }
  i_food.set_data("list_crop_name",  crop_names);
  i_food.set_data("list_crop_type",  crop_types);
  i_food.set_data("list_crop_food",  crop_food );
  i_food.set_data("list_crop_grown", crop_grown);
}

void Interface::minister_morale()
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
