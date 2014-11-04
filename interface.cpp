#include "interface.h"
#include "window.h"
#include "cuss.h"
#include "city.h"
#include "player_city.h"
#include "stringfunc.h"
#include "keys.h"
#include "building.h"
#include <sstream>
#include <cstdarg> // For the variadic function below
#include <map>

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

bool Interface::init(Game* G, Player_city* C)
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
"Hunting",
"Mining",
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
    std::stringstream ss_race;
    Race_datum* race_dat = Race_data[city->get_race()];
    ss_race << "<c=" << color_tag(race_dat->color) << ">" <<
               capitalize(race_dat->plural_name) << "<c=/>";
    i_main.set_data("text_race", ss_race.str());
    city->draw_map(i_main.find_by_name("draw_map"), sel, city_radius,
                   show_terrain);
    i_main.set_data("num_population",   city->get_total_population());
    i_main.set_data("num_gold",         city->get_resource_amount(RES_GOLD) );
    i_main.set_data("num_food",         city->get_resource_amount(RES_FOOD) );
    i_main.set_data("num_wood",         city->get_resource_amount(RES_WOOD) );
    i_main.set_data("num_stone",        city->get_resource_amount(RES_STONE));

    if (cur_mode == IMODE_VIEW_MAP && current_area != AREA_NULL) {
      display_area_stats(current_area);
    } else {
      i_main.set_data("text_map_info", city->get_map_info(sel));
    }

    i_main.draw(&w_main);
    w_main.refresh();
    long ch = input();

    restore_info_text();  // Undo temporary change to text_map_info

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

    if (ch == '*') {
      debugmsg("mode %d", cur_mode);
    }

    switch (cur_mode) {

      case IMODE_MENU:
// Most menu stuff is handled above (keys '1' - '9').  But Q can close the menu.
        if (ch == 'q' || ch == 'Q') {
          set_menu(MENU_NULL);
          set_mode(IMODE_VIEW_MAP);
        }
        break;

      case IMODE_VIEW_MAP: {
        Point p = input_direction(ch);

// Move the cursor
        if (p.x != -2 && (p.x != 0 || p.y != 0)) {
          sel += p;
          if (sel.x < 0) {
            sel.x = 0;
          } else if (sel.x >= CITY_MAP_SIZE) {
            sel.x = CITY_MAP_SIZE - 1;
          }
          if (sel.y < 0) {
            sel.y = 0;
          } else if (sel.y >= CITY_MAP_SIZE) {
            sel.y = CITY_MAP_SIZE - 1;
          }

// Get info on currently-selected tile
        } else if (ch == '\n') {
          if (current_area != AREA_NULL) {
            enqueue_area();
          } else {
            popup( city->get_map_info(sel).c_str() );
          }

// Revert to normal mode (not building an area, VIEW_MAP mode)
        } else if (ch == 'q' || ch == 'Q') {
          current_area = AREA_NULL;
          set_mode(IMODE_VIEW_MAP);
          i_main.clear_data("text_data");

// Toggle hiding areas (i.e. only show terrain)
        } else if (ch == 't' || ch == 'T') {
          show_terrain = !show_terrain;

// Toggled grayed-out tiles outside of our city's radius
        } else if (ch == 'r' || ch == 'R') {
          city_radius = !city_radius;

// Build a new area
        } else if (ch == 'a' || ch == 'A') {
          current_area = pick_area();
          display_area_stats(current_area);
          set_mode(IMODE_VIEW_MAP);
          Building_datum* build = get_building_for(current_area);
          if (current_area != AREA_NULL && build) {
            i_main.set_data("text_data", build->get_short_description());
          }

// Close an area
        } else if (ch == 'c' || ch == 'C') {
          Area* area_selected = city->area_at(sel);
          if (area_selected && area_selected->open &&
              query_yn("Really close your %s?",
                       area_selected->get_name().c_str())) {
            area_selected->close(city);
          }

// Destroy an area
        } else if (ch == 'd' || ch == 'D') {
          Area* area_selected = city->area_at(sel);
          if (area_selected) {
            int cost = area_selected->get_building_datum()->destroy_cost;
            int gold = city->get_resource_amount(RES_GOLD);

            if (gold < cost) {
              popup("Destroying that %s costs <c=ltred>%d<c=/> gold.  You have \
<c=ltred>%d<c=/> gold.", area_selected->get_name().c_str(), cost, gold);

            } else if (query_yn("Destroy the %s?\nCost: %d gold\nYou:  %d gold",
                               area_selected->get_name().c_str(), cost, gold)) {
              city->destroy_area_at(sel);
            }
          }

// Move time forward by 1 day
        } else if (ch == '.') {
          game->advance_time(1, city);

// Move time forward by 1 week
        } else if (ch == '>') {
          game->advance_time(7, city);
        }

      } break;

      default:
        break;
          
// TODO: Fill this in with all keybindings for all modes.
    }
  }
}

void Interface::set_mode(Interface_mode mode)
{
  cur_mode = mode;
  i_main.clear_data("text_map_info"); // TODO: No?
  switch (mode) {

    case IMODE_NULL:
    case IMODE_MENU:
      i_main.clear_data("text_commands");
      break;

    case IMODE_VIEW_MAP: {
      std::stringstream commands;
      commands << "<c=pink>Movement keys<c=/>: Scroll" << std::endl;
      if (current_area != AREA_NULL) {
        commands << "<c=pink>Enter<c=/>: Place " <<
                    Area_data[current_area]->name << std::endl <<
                    "<c=pink>Q<c=/>: Cancel " <<
                    Area_data[current_area]->name << " placement" << std::endl;
      } else {
        commands << "<c=pink>Enter<c=/>: Get info on tile" << std::endl;
      }
      commands << "<c=pink>T<c=/>: Toggle terrain view" << std::endl;
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

  } // switch (mode)

}

void Interface::set_menu(Menu_id item)
{
  std::string menu_name;
  int posx = -1;
// If we're setting the currently-open menu, instead just close it.
  if (item == cur_menu) {
    item = MENU_NULL;
  } else if (item != MENU_NULL) {
    set_mode(IMODE_MENU);
  }
// Remove any currently-open menu.
  if (cur_menu != MENU_NULL) {
    get_menu_info(cur_menu, menu_name, posx);
    i_main.erase_element(menu_name);  // Delete the element created for old menu
    i_main.erase_element("menu_border");  // Delete the border created for "
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
          if (query_yn("Really quit?")) {
            game_state = GAME_QUIT;
          }
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
        case 3: // Master of the Hunt
          minister_hunt();
          break;
        case 4: // Mining minister
          minister_mining();
          break;
        case 5: // Morale minister
          minister_morale();
          break;
      }
      break;

    case MENU_BUILDINGS:
      switch (index) {
        case 1: // View buildings
          building_status();
          break;
        case 2: // Build building
          build_building();
          break;
      }
      break;

// TODO: Other menus.

  }
}

void Interface::set_temp_info(std::string text)
{
  original_info_text = i_main.get_str("text_map_info");
  i_main.set_data("text_map_info", text);
}

void Interface::restore_info_text()
{
  if (!original_info_text.empty()) {
    i_main.set_data("text_map_info", original_info_text);
    original_info_text = "";
  }
}

void Interface::display_area_stats(Area_type type)
{
  i_main.clear_data("text_map_info");

  std::stringstream stats; // We'll set text_data to this

  Building_datum* build_dat = Area_data[type]->get_building_datum();
  std::string area_name = Area_data[type]->name;

  Map_tile* tile = city->map.get_tile(sel);
  Terrain_datum* ter_dat = city->map.get_terrain_datum(sel);

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
      stats << tile->get_terrain_name() << " farmability: " <<
               tile->get_farmability() << "%%%%" << std::endl;
      stats << std::endl;
      stats << "Crops here: " << tile->get_crop_info() << std::endl;
      stats << "Food consumed each day: " << city->get_food_consumption() <<
               std::endl;
      stats << "Food produced each day: " << city->get_food_production();
      break;

    case AREA_HUNTING_CAMP:
      stats << "Game in this " << tile->get_terrain_name() << ":" << std::endl;
      stats << tile->get_animals_info() << std::endl;
      stats << "Food consumed each day: " << city->get_food_consumption() <<
               std::endl;
      stats << "Food produced each day: " << city->get_food_production();
      break;

    case AREA_MINE:
      if (ter_dat->minerals.empty()) {
        stats << "<c=dkgray>" << ter_dat->name << " does not contain any " <<
                 "minerals." << "<c=/>";
      } else {
        stats << ter_dat->name << " <c=white>may<c=/> contain: " << std::endl;
        for (int i = 0; i < ter_dat->minerals.size(); i++) {
          Mineral_amount min_amt = ter_dat->minerals[i];
          Mineral_datum* min_dat = Mineral_data[min_amt.type];
          stats << capitalize( mineral_amount_ranking(min_amt) ) << " <c=" << 
                   color_tag(min_dat->color) << ">" << min_dat->name << "<c=/>";
          if (i < ter_dat->minerals.size() - 1) {
            stats << "," << std::endl;
          }
        }
      }
      break;

    case AREA_SAWMILL:
      stats << capitalize(trees_amount_ranking(tile->wood)) << " trees.";
      break;

    case AREA_BARRACKS:
      stats << "Number of soldiers: " << city->get_military_count() <<
               std::endl;
      stats << "Soldiers supported: " << city->get_military_supported();
      break;
  }

  i_main.set_data("text_map_info", stats.str());
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
  cuss::interface i_finance;
  if (!i_finance.load_from_file("cuss/finance.cuss")) {
    return;
  }
  Window w_finance(0, 0, 80, 24);

  bool done = false;

  int current_gold = city->get_resource_amount(RES_GOLD);
  i_finance.ref_data("num_gold", &current_gold);

  int income_taxes = 0, income_trade = 0, income_mint = 0, income_total = 0;
  i_finance.ref_data("num_income",        &income_total);
  i_finance.ref_data("num_income_taxes",  &income_taxes);
  i_finance.ref_data("num_income_trade",  &income_trade);
  i_finance.ref_data("num_income_mint",   &income_mint );

  int expense_wages = 0, expense_trade = 0, expense_maintenance = 0,
      expense_corruption = 0, expense_military = 0, expense_total = 0;
  i_finance.ref_data("num_expenses",            &expense_total);
  i_finance.ref_data("num_expense_wages",       &expense_wages);
  i_finance.ref_data("num_expense_trade",       &expense_trade);
  i_finance.ref_data("num_expense_maintenance", &expense_maintenance);
  i_finance.ref_data("num_expense_corruption",  &expense_corruption);
  i_finance.ref_data("num_expense_military",    &expense_military);

  int net_income = 0;
  i_finance.ref_data("num_net", &net_income);

  int citizen_wealth  [CIT_MAX];
  int citizen_income  [CIT_MAX];
  int citizen_tax_rate[CIT_MAX];
  int citizen_taxes   [CIT_MAX];
  for (int i = 1; i < CIT_MAX; i++) {
    Citizen_type cit_type = Citizen_type(i);
    Citizens* citizens = &(city->population[cit_type]);
    std::stringstream wealth_name, income_name, tax_rate_name, taxes_name;

    wealth_name   << "num_wealth_"   << citizen_type_name(cit_type);
    income_name   << "num_income_"   << citizen_type_name(cit_type);
    tax_rate_name << "num_tax_rate_" << citizen_type_name(cit_type);
    taxes_name    << "num_taxes_"    << citizen_type_name(cit_type);

    citizen_wealth  [i] = citizens->wealth;
    citizen_income  [i] = citizens->get_income();
    citizen_tax_rate[i] = city->tax_rate[cit_type];
    citizen_taxes   [i] = city->get_taxes(cit_type);

    i_finance.ref_data(wealth_name.str(),   &(citizen_wealth  [i]));
    i_finance.ref_data(income_name.str(),   &(citizen_income  [i]));
    i_finance.ref_data(tax_rate_name.str(), &(citizen_tax_rate[i]));
    i_finance.ref_data(taxes_name.str(),    &(citizen_taxes   [i]));

    income_taxes += citizen_taxes[i];
  }

// TODO: Calculate trade income & expenses

  income_mint = city->get_resource_production(RES_GOLD);

// We now know enough to calculate the total gross income...
  income_total = income_taxes + income_trade + income_mint;
// ... which lets us calculate the total lost to corruption.
  expense_corruption = income_total * city->get_corruption_percentage();
  expense_corruption /= 100;

// Calculate maintenace
  expense_maintenance = city->get_total_maintenance();

// Calculate wages
  expense_wages = city->get_total_wages();

// Military expenditure
  expense_military = city->get_military_expense();

// Now we have total gross expenses...
  expense_total = expense_wages + expense_trade + expense_maintenance +
                  expense_corruption + expense_military;
// ... and our net income.
  net_income = income_total - expense_total;

  i_finance.select("list_citizens");

  while (!done) {
// Set colors.
    if (current_gold == 0) {
      i_finance.set_data("num_gold", c_red);
    } else {
      i_finance.set_data("num_gold", c_ltgray);
    }

    if (income_total == 0) {
      i_finance.set_data("num_income", c_red);
    } else {
      i_finance.set_data("num_income", c_ltgreen);
    }

    if (expense_total == 0) {
      i_finance.set_data("num_expenses", c_ltblue);
    } else {
      i_finance.set_data("num_expenses", c_ltred);
    }

    if (net_income < 0) {
      i_finance.set_data("num_net", c_ltred);
    } else if (net_income > 0) {
      i_finance.set_data("num_net", c_ltgreen);
    } else {
      i_finance.set_data("num_net", c_blue);
    }

    if (income_taxes > 0) {
      i_finance.set_data("num_income_taxes", c_ltgreen);
    } else {
      i_finance.set_data("num_income_taxes", c_dkgray);
    }

    if (income_trade > 0) {
      i_finance.set_data("num_income_trade", c_ltgreen);
    } else {
      i_finance.set_data("num_income_trade", c_dkgray);
    }

    if (income_mint > 0) {
      i_finance.set_data("num_income_mint", c_ltgreen);
    } else {
      i_finance.set_data("num_income_mint", c_dkgray);
    }

    if (expense_wages > 0) {
      i_finance.set_data("num_expense_wages", c_ltred);
    } else {
      i_finance.set_data("num_expense_wages", c_dkgray);
    }

    if (expense_trade > 0) {
      i_finance.set_data("num_expense_trade", c_ltred);
    } else {
      i_finance.set_data("num_expense_trade", c_dkgray);
    }

    if (expense_maintenance > 0) {
      i_finance.set_data("num_expense_maintenance", c_ltred);
    } else {
      i_finance.set_data("num_expense_maintenance", c_dkgray);
    }

    if (expense_corruption > 0) {
      i_finance.set_data("num_expense_corruption", c_ltred);
    } else {
      i_finance.set_data("num_expense_corruption", c_dkgray);
    }

    if (expense_military > 0) {
      i_finance.set_data("num_expense_military", c_ltred);
    } else {
      i_finance.set_data("num_expense_military", c_dkgray);
    }

    i_finance.draw(&w_finance);
    w_finance.refresh();

    long ch = getch();

    switch (ch) {
      case 'q':
      case 'Q':
      case KEY_ESC:
        done = true;
        break;

      case '=':
      case '+': {
        int index = i_finance.get_int("list_citizens");
// Our list starts at index 0, which is peasants.  So we want to map 0 to
// peasants, 1 to peasants+1, etc.
        Citizen_type tax_type = Citizen_type( CIT_PEASANT + index );
        if (city->tax_rate[tax_type] < 100) {
          citizen_tax_rate[tax_type] += 5;
// Round down to nearest multiple of 5.
          citizen_tax_rate[tax_type] -= citizen_tax_rate[tax_type] % 5;
          city->set_tax_rate(tax_type, citizen_tax_rate[tax_type]);
// Remove the old money from taxes...
          income_taxes -= citizen_taxes[tax_type];
          income_total -= citizen_taxes[tax_type];
          net_income   -= citizen_taxes[tax_type];
// Fix the amount we get from taxes for this citizen type...
          citizen_taxes   [tax_type] = city->get_taxes(tax_type);
// ... and add in the new money from taxes.
          income_taxes += citizen_taxes[tax_type];
          income_total += citizen_taxes[tax_type];
          net_income   += citizen_taxes[tax_type];
// Set a warning if the rate is too high / too low
          std::stringstream field_name;
          field_name << "num_tax_rate_" << citizen_type_name(tax_type);
          if (citizen_tax_rate[tax_type] >= city->get_high_tax_rate(tax_type)) {
            i_finance.set_data(field_name.str(), c_ltred);
          } else if (citizen_tax_rate[tax_type] <=
                     city->get_low_tax_rate(tax_type)) {
            i_finance.set_data(field_name.str(), c_ltgreen);
          } else {
            i_finance.set_data(field_name.str(), c_ltgray);
          }
        }
      } break;

      case '-': {
        int index = i_finance.get_int("list_citizens");
// Our list starts at index 0, which is peasants.  So we want to map 0 to
// peasants, 1 to peasants+1, etc.
        Citizen_type tax_type = Citizen_type( CIT_PEASANT + index );
        if (city->tax_rate[tax_type] > 0) {
// Alter the rate...
          citizen_tax_rate[tax_type] -= 5;
// Round down to nearest multiple of 5.
          citizen_tax_rate[tax_type] -= citizen_tax_rate[tax_type] % 5;
          city->set_tax_rate(tax_type, citizen_tax_rate[tax_type]);
// Remove the old money from taxes...
          income_taxes -= citizen_taxes[tax_type];
          income_total -= citizen_taxes[tax_type];
          net_income   -= citizen_taxes[tax_type];
// Fix the amount we get from taxes for this citizen type...
          citizen_taxes   [tax_type] = city->get_taxes(tax_type);
// ... and add in the new money from taxes.
          income_taxes += citizen_taxes[tax_type];
          income_total += citizen_taxes[tax_type];
          net_income   += citizen_taxes[tax_type];
// Set a warning if the rate is too high / too low
          std::stringstream field_name;
          field_name << "num_tax_rate_" << citizen_type_name(tax_type);
          if (citizen_tax_rate[tax_type] >= city->get_high_tax_rate(tax_type)) {
            i_finance.set_data(field_name.str(), c_ltred);
          } else if (citizen_tax_rate[tax_type] <=
                     city->get_low_tax_rate(tax_type)) {
            i_finance.set_data(field_name.str(), c_ltgreen);
          } else {
            i_finance.set_data(field_name.str(), c_ltgray);
          }
        }
      } break;

      default:
        i_finance.handle_action(ch);
        break;
    }
  }
}

void Interface::minister_food()
{
  cuss::interface i_food;
  if (!i_food.load_from_file("cuss/food.cuss")) {
    return;
  }
  Window w_food(0, 0, 80, 24);

// These values and fields are static during the life of this interface.
  int num_farms     = city->get_number_of_buildings(BUILD_FARM);
  int food_stored   = city->get_resource_amount(RES_FOOD);
  int food_cap      = city->get_food_cap();
  int food_imported = city->get_import(RES_FOOD);
  int food_consumed = city->get_food_consumption();
  int food_exported = city->get_export(RES_FOOD);

  i_food.set_data("num_farms",         num_farms);
  if (num_farms == 0) {
    i_food.set_data("num_farms", c_red);
  }

  i_food.set_data("num_food_stored",   food_stored);
  i_food.set_data("num_food_cap",      food_cap);
  if (food_stored == 0) {
    i_food.set_data("num_food_stored", c_red);
  } else if (food_stored >= food_cap) {
    i_food.set_data("num_food_stored", c_blue);
    i_food.set_data("num_food_cap",    c_blue);
  } else if (food_stored >= food_cap * 0.9) {
    i_food.set_data("num_food_stored", c_ltblue);
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
      case '=':
      case '+': {
        bool did_it = false;  // Track if changes (& interface will need update)
        int crop_index = i_food.get_int("list_crop_name");
        crop_change++;  // If it was -2, it's now -1; otherwise, it's now +1

// These keys are only meaningful if list_crop_name is selected.
        if (i_food.selected()->name == "list_crop_name") {
          Building* farm_build = &(cur_farm->building);

// Validate our crop index...
          if (crop_index >= 0 && crop_index < farm_build->crops_grown.size()) {

            if (crop_change < 0) { // We're firing workers
              crop_change = 0 - crop_change;  // Absolute value that bitch

// To decrease farming, we have to be able to fire an appropriate number of
// peasants from this farm... so make sure we can!
              if (farm_build->crops_grown[crop_index].amount >= crop_change &&
                city->fire_citizens(CIT_PEASANT, crop_change, farm_build)) {
                farm_build->crops_grown[crop_index].amount -= crop_change;
                did_it = true;
              }

// To increase farming, we have to be able to hire and appropriate number of 
// peasants to work at this farm... so make sure we can!
            } else if (crop_change > 0 &&
                       farm_build->get_empty_fields() >= crop_change &&
                       city->employ_citizens(CIT_PEASANT, crop_change,
                                             farm_build)) {
              farm_build->crops_grown[crop_index].amount += crop_change;
              did_it = true;
            }

          } // crop_index validation
        } // if (i_food.selected()->name == "list_crop_name")

        if (did_it) { // We actually changed # of crops!  Update interface.
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
      } break; // "Changing # of crop" block

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

void Interface::minister_hunt()
{
  cuss::interface i_hunt;
  if (!i_hunt.load_from_file("cuss/hunting.cuss")) {
    return;
  }
  Window w_hunt(0, 0, 80, 24);

// Set up static data (left side)
  std::vector<std::string> names, actions, killed, livestock;

// Start at 1 to skip ANIMAL_NULL
  for (int i = 1; i < ANIMAL_MAX; i++) {
    Animal animal = Animal(i);
    names.push_back( capitalize( Animal_data[i]->name ) );
    actions.push_back( capitalize( animal_action_name(
                       city->get_hunting_action(animal)   ) ) );
    if (city->hunt_kills.count(animal)) {
      killed.push_back( itos( city->hunt_kills[animal] ) );
    } else {
      killed.push_back("0");
    }
    if (city->livestock.count(animal)) {
      livestock.push_back( itos( city->livestock[animal] ) );
    } else {
      livestock.push_back("0");
    }
  }

  i_hunt.set_data("list_animals", names);
  i_hunt.ref_data("list_action", &actions); // ref_data cause they may chance
  i_hunt.set_data("list_killed", killed);
  i_hunt.set_data("list_livestock", livestock);

// Start with ANIMAL_NULL so that our "check if we picked a new animal" check at
// the start of our loop will hit the first go-through.
  Animal cur_animal = ANIMAL_NULL;
  bool done = false;
  i_hunt.select("list_animals");

  while (!done) {
// Check if we picked a new animal.
// + 1 because it doesn't contain ANIMAL_NULL, so the 0th index item is actually
// Animal(1).
    Animal new_animal = Animal( i_hunt.get_int("list_animals") + 1 );
    if (new_animal != cur_animal) { // We changed!
      cur_animal = new_animal;
      Animal_datum* animal_dat = Animal_data[cur_animal];
      i_hunt.set_data("text_danger", animal_danger_ranking(animal_dat->danger));
      i_hunt.set_data("num_pack_size", animal_dat->max_pack_size);
      i_hunt.set_data("num_flee_chance", animal_dat->flee_chance);
      i_hunt.set_data("num_capture_chance", animal_dat->tameness);
      i_hunt.set_data("num_food_killed", animal_dat->food_killed);

      i_hunt.clear_data("list_resources_killed");

      if (animal_dat->resources_killed.empty()) {
        i_hunt.add_data("list_resourcs_killed", "<c=dkgray>None<c=/>");

      } else {
        for (int i = 0; i < animal_dat->resources_killed.size(); i++) {
          std::stringstream ss_res;
          Resource_amount res = animal_dat->resources_killed[i];
          ss_res << resource_name(res.type) << " x " << res.amount;
          i_hunt.add_data("list_resources_killed", ss_res.str());
        }
      }
// food_livestock and food_eaten are measured in food per 100 animals.  So, we
// need to divide it by 100 and display decimals.
      std::stringstream ss_food_daily, ss_food_eaten;

      ss_food_daily << animal_dat->food_livestock / 100 << ".";
      int food_decimal = animal_dat->food_livestock % 100;
      if (food_decimal < 10) {
        ss_food_daily << "0"; // So we get "1.07" not "1.7"
      }
      ss_food_daily << food_decimal;
      i_hunt.set_data("text_food_livestock",       ss_food_daily.str());

      ss_food_eaten << animal_dat->food_eaten / 100 << ".";
      food_decimal = animal_dat->food_eaten % 100;
      if (food_decimal < 10) {
        ss_food_eaten << "0"; // So we get "1.07" not "1.7"
      }
      ss_food_eaten << food_decimal;
      i_hunt.set_data("text_food_eaten_livestock", ss_food_eaten.str());

      if (animal_dat->carnivore) {
        i_hunt.set_data("text_diet", "<c=ltred>Carnivorous<c=/>");
      } else {
        i_hunt.set_data("text_diet", "<c=ltgreen>Will eat hay<c=/>");
      }

      i_hunt.clear_data("list_resources_livestock");

      if (animal_dat->resources_livestock.empty()) {
        i_hunt.add_data("list_resources_livestock", "<c=dkgray>None<c=/>");

      } else {
        for (int i = 0; i < animal_dat->resources_livestock.size(); i++) {
          std::stringstream ss_res;
          Resource_amount res = animal_dat->resources_livestock[i];
          ss_res << resource_name(res.type) << " x ";
// Like food_livestock, these amounts are per 100 animals.  Same solution.
          ss_res << res.amount / 100;
          int res_decimal = res.amount % 100;
          if (res_decimal < 10) {
            ss_res << "0"; // So we get "1.07" not "1.7"
          }
          ss_res << res_decimal;
          i_hunt.add_data("list_resources_livestock", ss_res.str());
        }
      }

    } // if (new_animal != cur_animal)

    i_hunt.draw(&w_hunt);
    w_hunt.refresh();

    long ch = input();

    switch (ch) {

      case KEY_ESC:
      case 'q':
      case 'Q':
        done = true;
        break;

      case 'a':
      case 'A': {
        Animal_action act = city->get_hunting_action(cur_animal);
        act = Animal_action(act + 1);
        if (act == ANIMAL_ACT_MAX) {
          act = Animal_action(1);
        }
// Animal(1) is index 0, so subtract 1 from the animal to get index for actions
        actions[cur_animal - 1] = capitalize( animal_action_name( act ) );
        city->set_hunting_action(cur_animal, act);
      } break;

      default: {
        i_hunt.handle_keypress(ch);
// Make sure all our lists line up together.
// TODO: This is messy.  cuss::interface should handle this for us.
        cuss::ele_list* list_animals =
          static_cast<cuss::ele_list*>(i_hunt.find_by_name("list_animals"));
        cuss::ele_list* list_action =
          static_cast<cuss::ele_list*>(i_hunt.find_by_name("list_action"));
        cuss::ele_list* list_killed =
          static_cast<cuss::ele_list*>(i_hunt.find_by_name("list_killed"));
        cuss::ele_list* list_livestock =
          static_cast<cuss::ele_list*>(i_hunt.find_by_name("list_livestock"));
        list_action->offset    = list_animals->offset;
        list_killed->offset    = list_animals->offset;
        list_livestock->offset = list_animals->offset;
      } break;
    } // switch (ch)

  } // while (!done)

}

void Interface::minister_mining()
{
  cuss::interface i_mining;
  if (!i_mining.load_from_file("cuss/mining.cuss")) {
    return;
  }
  Window w_mining(0, 0, 80, 24);

  int num_mines = city->get_number_of_buildings(BUILD_MINE);
  i_mining.set_data("num_mines", num_mines);
  if (num_mines == 0) {
    i_mining.set_data("num_mines", c_red);
  }

// Set up a list of mines.
  std::vector<std::string> mine_terrain, mine_shafts;
  std::vector<Area*> mines;
  for (int i = 0; i < city->areas.size(); i++) {
    if (city->areas[i].produces_resource(RES_MINING)) {
      Point area_loc = city->areas[i].pos;
      Building* build = &(city->areas[i].building);

      std::stringstream empty_shafts_ss;
      int empty_shafts = build->get_empty_shafts();
      if (empty_shafts > 0) {
        empty_shafts_ss << "<c=ltgreen>" << empty_shafts << "<c=/>";
      } else {
        empty_shafts_ss << empty_shafts;
      }

      std::stringstream mine_terrain_ss;
      std::string terrain_name = city->map.get_terrain_name(area_loc);

      mines.push_back( &(city->areas[i]) );
      mine_terrain.push_back( terrain_name );
      mine_shafts.push_back( empty_shafts_ss.str() );
    }
  }

  i_mining.ref_data("list_mines",       &mine_terrain);
  i_mining.ref_data("list_free_shafts", &mine_shafts);

/* We also need to list minerals mined at the currently-selected mine.
 * We always start by selecting mines[0]; later on, if we select a new mine,
 * we'll update the list.  We don't want to update the list on every cycle of
 * the loop because that'll clear list_mineral_name and move the cursor in that
 * list back to 0, making it impossible to select any other mineral.
 */
  int mine_index = 0;
  Area* cur_mine = NULL;
/* Since several of a mine's minerals may be hidden, they won't be displayed
 * in the interface.  Thus, we need a way to link the ones that are displayed to
 * the actual indices of the Mineral_amounts in the building.  mineral_indices
 * does that job.  Its size is the same as the displayed minerals, and each int
 * in the vector is the index of the actual Mineral_amount from the building.
 * Example: assuming the first two minerals are hidden, mineral_indices[0] will
 * store 2 - because the first displayed mineral refers to the third stored
 * mineral.
 * list_mine_minerals() sets mineral_indices.
 */
  std::vector<int> mineral_indices;
  if (!mines.empty()) {
    cur_mine = mines[ mine_index ];
    list_mine_minerals(cur_mine, mineral_indices, i_mining);
  }

  std::map<Mineral,int> minerals_used = city->get_minerals_used();

// Minerals used shouldn't change during this function, so we can set it here.
  for (int i = 1; i < MINERAL_MAX; i++) {
    Mineral mineral = Mineral(i);
    Mineral_datum* min_dat = Mineral_data[mineral];

    std::stringstream mineral_ss;
    mineral_ss << "<c=" << color_tag(min_dat->color) << ">" << min_dat->name <<
                  "<c=/>";
    i_mining.add_data("list_master_minerals", mineral_ss.str());

    std::stringstream used_ss, stored_ss;
    int amount = (minerals_used.count(mineral) ? minerals_used[mineral] : 0);
    if (amount == 0) {
      used_ss << "<c=dkgray>";
    }
    used_ss << amount << "<c=/>";

    int amount_stored = city->get_mineral_amount(mineral);
    if (amount_stored == 0) {
      stored_ss << "<c=dkgray>";
    }
    stored_ss << amount_stored << "<c=/>";

    i_mining.add_data("list_total_used",   used_ss.str()  );
    i_mining.add_data("list_total_stored", stored_ss.str());
  }

  i_mining.select("list_mines");

  bool done = false;

  while (!done) {
    int shafts_worked = city->get_shafts_worked();
    int free_shafts   = city->get_free_shafts();
    int free_peasants = city->get_unemployed_citizens(CIT_PEASANT);

    i_mining.set_data("num_shafts_worked", shafts_worked);
    if (shafts_worked == 0 && num_mines > 0) {
      i_mining.set_data("num_shafts_worked", c_red);
    } else if (num_mines == 0) {
      i_mining.set_data("num_shafts_worked", c_dkgray);
    } else {
      i_mining.set_data("num_shafts_worked", c_ltgray);
    }

    i_mining.set_data("num_free_shafts", free_shafts);
    if (free_shafts > 0 && num_mines > 0) {
      i_mining.set_data("num_free_shafts", c_yellow);
    } else if (num_mines == 0) {
      i_mining.set_data("num_free_shafts", c_dkgray);
    } else {
      i_mining.set_data("num_free_shafts", c_ltgray);
    }
    
    i_mining.set_data("num_free_peasants", free_peasants);
    if (free_shafts == 0) {  // We don't care since all shafts are used
      i_mining.set_data("num_free_peasants", c_dkgray);
    } else {  // We care since we could utilize a shaft
      if (free_peasants == 0) {
        i_mining.set_data("num_free_peasants", c_red);
      } else {
        i_mining.set_data("num_free_peasants", c_ltgreen);
      }
    }

// Check if we selected a new mine.
    int new_mine_index = i_mining.get_int("list_mines");
    if (new_mine_index != mine_index && new_mine_index >= 0 &&
        new_mine_index < mines.size()) {
      mine_index = new_mine_index;
      cur_mine = mines[mine_index];
      list_mine_minerals(cur_mine, mineral_indices, i_mining);
    }

// Set up amount mined
    i_mining.clear_data("list_total_mined");
    for (int i = 1; i < MINERAL_MAX; i++) {
      Mineral min = Mineral(i);
      int amount = city->get_amount_mined(min);
      std::stringstream amount_ss;
      if (amount == 0) {
        amount_ss << "<c=dkgray>";
      }
      amount_ss << amount << "<c=/>";
      i_mining.add_data("list_total_mined", amount_ss.str());
    }

    i_mining.draw(&w_mining);
    w_mining.refresh();

    int mineral_change = 0;
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
        mineral_change = -2;
// Intentionally fall through...

      case KEY_RIGHT:
      case 'l':
      case 'L':
      case '=':
      case '+': {
        bool did_it = false;  // Track if changes (& interface will need update)
        int mineral_index = i_mining.get_int("list_mineral_name");
        mineral_change++;  // If it was -2, it's now -1; otherwise, it's now +1
// These keys are only meaningful if list_mineral_name is selected.
        if (i_mining.selected()->name == "list_mineral_name") {
          Building* mine_build = &(cur_mine->building);

// Validate our mineral index
          if (mineral_index >= 0 && mineral_index < mineral_indices.size()) {

// The Mineral_amount we're changing - we need to refer to mineral_indices to
// get the actual index for the building!
            int ind = mineral_indices[mineral_index];
            Mineral_amount* mining_changed = &(mine_build->minerals_mined[ind]);

            if (mineral_change < 0) { // Decreasing amount mined
              mineral_change = 0 - mineral_change;  // Absolute value that bitch

// To decrease mining, we have to be able to fire an appropriate number of
// peasants from this mine... so make sure we can!
              if (mining_changed->amount >= mineral_change &&
                city->fire_citizens(CIT_PEASANT, mineral_change, mine_build)) {
                mining_changed->amount -= mineral_change;
                did_it = true;
              }

// To increase mining, we have to be able to hire an appropriate number of
// peasants to work at this mine... so make sure we can!
            } else if (mineral_change > 0 &&
                       mine_build->get_empty_shafts() >= mineral_change &&
                       city->employ_citizens(CIT_PEASANT, mineral_change,
                                             mine_build)) {
              mining_changed->amount += mineral_change;
              did_it = true;
            }

          } // mineral index validation
        } // if (i_mining.selected()->name == "list_mineral_name")

        if (did_it) { // We actually changed a # of minerals!  Update interface.
          list_mine_minerals(cur_mine, mineral_indices, i_mining);
// Reset our position in the list to what it was previously!
          i_mining.set_data("list_mineral_name", mineral_index);
// Fix the list of empty shafts
          std::stringstream empty_shafts_ss;
          Building* build = &(mines[mine_index]->building);
          int empty_shafts = build->get_empty_shafts();
          if (empty_shafts > 0) {
            empty_shafts_ss << "<c=ltgreen>" << empty_shafts << "<c=/>";
          } else {
            empty_shafts_ss << empty_shafts;
          }
          mine_shafts[mine_index] = empty_shafts_ss.str();
        }
      } break; // "Changing # of minerals" block
          
          
      default:
        i_mining.handle_action(ch);
        break;
    }
  }

}

void Interface::list_mine_minerals(Area* cur_mine,
                                   std::vector<int>& mineral_indices,
                                   cuss::interface& i_mining)
{
// Clear mineral_indices - we'll be setting it up from scratch
  mineral_indices.clear();
// List minerals mined at cur_mine
  std::vector<std::string> mineral_names, mineral_value, mineral_mined;
  Building* mine_build = &(cur_mine->building);
  for (int i = 0; i < mine_build->minerals_mined.size(); i++) {
    Mineral_amount cur_mineral = mine_build->minerals_mined[i];
    if (cur_mineral.amount != HIDDEN_RESOURCE) {
      Mineral_datum* mineral_dat = Mineral_data[cur_mineral.type];

// TODO: Adjust value due to market forces?

      int amount = cur_mineral.amount * mine_build->shaft_output;
      mineral_indices.push_back(i);
      mineral_names.push_back(mineral_dat->name);
      mineral_value.push_back( itos( mineral_dat->value ) );
      mineral_mined.push_back( itos( amount ) );
    }
  }
  i_mining.set_data("list_mineral_name",  mineral_names);
  i_mining.set_data("list_mineral_value", mineral_value );
  i_mining.set_data("list_mineral_mined", mineral_mined);
}

void Interface::minister_morale()
{
}

void Interface::building_status()
{
  cuss::interface i_buildings;
  if (!i_buildings.load_from_file("cuss/buildings.cuss")) {
    return;
  }
  Window w_buildings(0, 0, 80, 24);

  std::vector<Building*> buildings = city->get_all_buildings();

  std::vector<std::string> building_names, workers, max_workers, worker_class;
  std::vector<bool> closed;

// Go through all the buildings and fill our string vectors with their info
  for (int i = 0; i < buildings.size(); i++) {
    Building* bldg = buildings[i];
    std::stringstream name_ss, workers_ss, max_workers_ss, worker_class_ss;
    
    name_ss << capitalize( bldg->get_name() );
    if (bldg->pos.x != -1) {  // It's an area building
// TODO: This is messy.  Better to track whether the building itself is closed.
      Area* bldg_area = city->area_at(bldg->pos);
      if (bldg_area && !bldg_area->open) {
        closed.push_back(true);
        name_ss << " <c=red>(Closed)<c=/>";
      } else {
        closed.push_back(false);
        name_ss << " (" << city->map.get_terrain_name(bldg->pos) << ")";
      }
    }

// Gray out worker-related lines if the building doesn't employ workers
    if (bldg->get_total_jobs() == 0) {
      workers_ss      << "<c=dkgray>";
      max_workers_ss  << "<c=dkgray>";
      worker_class_ss << "<c=dkgray>";
    } else if (bldg->workers == 0) {
      workers_ss << "<c=red>";
    }

    workers_ss     << bldg->workers          << "<c=/>";
    max_workers_ss << bldg->get_total_jobs() << "<c=/>";

    if (bldg->get_total_jobs() == 0) {
      worker_class_ss << "N/A";
    } else {
      Citizen_type cit_type = bldg->get_job_citizen_type();
      worker_class_ss << capitalize( citizen_type_name( cit_type ) ) << "<c=/>";
    }

    building_names.push_back(name_ss.str()        );
    workers.push_back       (workers_ss.str()     );
    max_workers.push_back   (max_workers_ss.str() );
    worker_class.push_back  (worker_class_ss.str());
  } // for (int i = 0; i < buildings.size(); i++)

/* Update the interface using the data we just fetched
 * Most are static during the life of this function, so we can use set_data().
 * However, the number of workers employed may change (since we can hire/fire
 * workers on this screen), so use ref_data() instead - that way, as soon as we
 * update the workers vector, the interface will update as well.
 */
  i_buildings.set_data("list_building_names", building_names);
  i_buildings.ref_data("list_workers",        &workers      );
  i_buildings.set_data("list_max_workers",    max_workers   );
  i_buildings.set_data("list_worker_class",   worker_class  );

// Set the fields that indicate our number of unemployed citizens of each class
  int free_peasants  = city->get_unemployed_citizens(CIT_PEASANT );
  int free_merchants = city->get_unemployed_citizens(CIT_MERCHANT);
  int free_burghers  = city->get_unemployed_citizens(CIT_BURGHER );
  i_buildings.ref_data("num_free_peasants",  &free_peasants );
  i_buildings.ref_data("num_free_merchants", &free_merchants);
  i_buildings.ref_data("num_free_burghers",  &free_burghers );

// Set up a few variables & prep the interface for our loop
  int index = -1; // -1 so that we'll update it on the first iteration

/* If this variable is not -1, it will move our selection in list_benefits to
 * its value when we update the list.  This is used when moving items around in
 * list_benefits, to make sure our cursor stays with the item that we moved.
 */
  int move_index = -1;

// Set a pointer to the currently-selected building (if our list isn't empty).
  Building* cur_bldg = NULL;
  if (!buildings.empty()) {
    cur_bldg = buildings[0];
  }

  bool done = false; // Once this is true, we exit the function.

/* If adjusting_production is true, then we're currently in the production list
 * and can add/remove items.  If it's false, we're currently selecting a
 * building.
 */
  bool adjusting_production = false;

/* If this is true, we need to update list_benefits (and then set it back to
 * false).  It will be set to true if we select a new building, or if we change
 * the contents of list_benefits (e.g. by adding or removing a production item).
 */
  bool update_benefits = false;

// Set our help text
  i_buildings.set_data("text_help", "\
<c=pink>Right<c=/>/<c=pink>L<c=/>/<c=pink>+<c=/>: Hire Citizen\n\
<c=pink>Left <c=/>/<c=pink>H<c=/>/<c=pink>-<c=/>: Fire Citizen\n\
<c=pink>TAB<c=/>: Edit production queue\n\
<c=pink>Q<c=/>: Leave this screen\
");

// Start our control in the list of buildings
  i_buildings.select("list_building_names");

// Start our loop!
  while (!done) {
// Check if we selected a new building
    int new_index = i_buildings.get_int("list_building_names");

    if (new_index != index && new_index >= 0 && new_index < buildings.size()) {
// We selected a new building!  Update information.
      index = new_index;
      cur_bldg = buildings[index];

      std::stringstream upkeep_ss;
      int upkeep = cur_bldg->get_upkeep();
      if (upkeep == 0) {
        upkeep_ss << "<c=dkgray>0.0<c=/>";
      } else if (closed[index]) {
        upkeep_ss << "<c=ltgray>---<c=/>";
      } else {
// Upkeep is reported in 1/10th of a gold, so display "13" as "1.3"
        upkeep_ss << "<c=red>" << upkeep / 10 << "." << upkeep % 10;
      }
      i_buildings.set_data("text_upkeep", upkeep_ss.str());

      i_buildings.clear_data("text_benefits_label");
      i_buildings.clear_data("list_benefits");
// Mark us as needing to update list_benefits, since we just cleared it out
      update_benefits = true;
    }

    if (update_benefits && cur_bldg) {
// Set update_benefits to false so we don't update every loop.
      update_benefits = false;

/* Use some if statements to figure out what the building does - we'll put the
 * relevent information in list_benefits, and label it properly using
 * text_benefits_label.
 * TODO: This assumes we don't offer jobs AND housing - may not always be true
 */
      Building_datum* bldg_dat = cur_bldg->get_building_datum();

      if (cur_bldg->produces_resource()) { // Any raw production

        i_buildings.set_data("text_benefits_label", "<c=yellow>Produces:<c=/>");
        std::vector<std::string> production_list;
        for (int i = 0; i < bldg_dat->production.size(); i++) {
          Resource_amount res_amt = bldg_dat->production[i];
          std::stringstream production_ss;
          production_ss << "<c=ltgray>" <<
                           capitalize( resource_name(res_amt.type) ) << " x " <<
                           res_amt.amount << "<c=/>";
          production_list.push_back( production_ss.str() );
        }
        i_buildings.set_data("list_benefits", production_list);

      } else if (cur_bldg->builds_resource()) { // Any manufacturing

        i_buildings.set_data("text_benefits_label",
                             "<c=yellow>Build Queue:<c=/>");
        std::vector<std::string> production_list;

        if (cur_bldg->build_queue.empty()) {  // Not producing anything!
          production_list.push_back("<c=ltred>Nothing!<c=/>");

        } else {
          for (int i = 0; i < cur_bldg->build_queue.size(); i++) {
            std::stringstream production_ss;
            Recipe_amount recipe = cur_bldg->build_queue[i];
            production_ss << "<c=ltgray>" <<
                             capitalize(resource_name(recipe.get_resource()));
            if (recipe.amount == INFINITE_RESOURCE) {
              production_ss << " <c=ltblue>(Infinite)<c=/>";
            } else {
              production_ss <<  " x " << recipe.amount << "<c=/>";
            }
            production_list.push_back( production_ss.str() );
          }
        }

        i_buildings.set_data("list_benefits", production_list);

// Check move_index to see if we need to set the selected item
        if (move_index != -1) {
          if (move_index >= production_list.size()) {
            move_index = production_list.size() - 1;
          }
          if (move_index < 0) {
            move_index = 0;
          }
          i_buildings.set_data("list_benefits", move_index);
          move_index = -1;
        }

      } else if (cur_bldg->get_housing() > 0) { // Any housing

        i_buildings.set_data("text_benefits_label", "<c=yellow>Housing:<c=/>");
        std::vector<std::string> housing_list;
        for (int i = 0; i < bldg_dat->housing.size(); i++) {
          Citizen_amount cit_amt = bldg_dat->housing[i];
          std::stringstream housing_ss;
          housing_ss << "<c=ltgray>" <<
                        capitalize( citizen_type_name(cit_amt.type) ) << ": " <<
                        cit_amt.amount << "<c=/>";
          housing_list.push_back( housing_ss.str() );
        }
        i_buildings.set_data("list_benefits", housing_list);

      } // TODO: An }else{ block for buildings that do none of the above?
    } // End of updating info on newly-selected building

    i_buildings.draw(&w_buildings);
    w_buildings.refresh();

    long ch = input();

// - and + can be used in the production queue; easiest way is to change it here
    if (adjusting_production) {
      if (ch == '-') {
        ch = 'r';
      } else if (ch == '+' || ch == '=') {
        ch = 'a';
      }
    }

    switch (ch) {
// Leave this screen
      case 'q':
      case 'Q':
      case KEY_ESC:
        done = true;
        break;

// Hire workers
      case KEY_RIGHT:
      case 'l':
      case 'L':
      case '=':
      case '+':
        if (cur_bldg) { // Safety check
// Can't add workers for mines or farms - that's done via ministers
          if (cur_bldg->type == BUILD_FARM) {
            popup("To add workers to a farm, use the Minister of Food.");

          } else if (cur_bldg->type == BUILD_MINE) {
            popup("To add workers to a mine, use the Minister of Mining.");

          } else {
            Citizen_type cit_type = cur_bldg->get_job_citizen_type();
            if (city->employ_citizens(cit_type, 1, cur_bldg)) {
              std::stringstream workers_ss;
              if (cur_bldg->get_total_jobs() == 0) {
                workers_ss << "<c=dkgray>";
              } else if (cur_bldg->workers == 0) {
                workers_ss << "<c=red>";
              }
              workers_ss << cur_bldg->workers          << "<c=/>";
              workers[index] = workers_ss.str();

              switch (cit_type) {
                case CIT_PEASANT:   free_peasants--;  break;
                case CIT_MERCHANT:  free_merchants--; break;
                case CIT_BURGHER:   free_burghers--;  break;
              }
            } // if (city->employ_citizens(cit_type, 1, cur_bldg))
          }
        } // if (cur_bldg)
        break;

// Fire workers
      case KEY_LEFT:
      case 'h':
      case 'H':
      case '-':
        if (cur_bldg) { // Safety check
// Can't add workers for mines or farms - that's done via ministers
          if (cur_bldg->type == BUILD_FARM) {
            popup("To remove workers from a farm, use the Minister of Food.");

          } else if (cur_bldg->type == BUILD_MINE) {
            popup("To remove workers from a mine, use the Minister of Mining.");

          } else {
            Citizen_type cit_type = cur_bldg->get_job_citizen_type();
            if (city->fire_citizens(cit_type, 1, cur_bldg)) {
              std::stringstream workers_ss;
              if (cur_bldg->get_total_jobs() == 0) {
                workers_ss << "<c=dkgray>";
              } else if (cur_bldg->workers == 0) {
                workers_ss << "<c=red>";
              }
              workers_ss << cur_bldg->workers          << "<c=/>";
              workers[index] = workers_ss.str();
              switch (cit_type) {
                case CIT_PEASANT:   free_peasants++;  break;
                case CIT_MERCHANT:  free_merchants++; break;
                case CIT_BURGHER:   free_burghers++;  break;
              }
            } // if (city->fire_citizens(cit_type, 1, cur_bldg))
          }
        } // if (cur_bldg)
        break;

// Move control between building selecting & production adjustment
      case '\t':
// Only allow for adjusting production for buildings that produce things
        if (cur_bldg && cur_bldg->builds_resource()) {
          adjusting_production = !adjusting_production;

          if (adjusting_production) {
            i_buildings.select("list_benefits");

// Set our help text
            i_buildings.set_data("text_help", "\
<c=pink>A<c=/>/<c=pink>+<c=/>: Add production item\n\
<c=pink>R<c=/>/<c=pink>-<c=/>: Remove production item\n\
<c=pink><<c=/>  : Move item up\n\
<c=pink>><c=/>  : Move item down\n\
<c=pink>TAB<c=/>: Return to building selection\n\
<c=pink>ESC<c=/>/<c=pink>Q<c=/>: Leave this screen\
");

          } else {
            i_buildings.select("list_building_names");

// Set our help text (TODO: this is a duplicate of the code just before the
//                          loop, don't do that)
            i_buildings.set_data("text_help", "\
<c=pink>Right<c=/>/<c=pink>L<c=/>/<c=pink>+<c=/>: Hire Citizen\n\
<c=pink>Left <c=/>/<c=pink>H<c=/>/<c=pink>-<c=/>: Fire Citizen\n\
<c=pink>TAB<c=/>: Edit production queue\n\
<c=pink>Q<c=/>: Leave this screen\
");
          }
        }
        break;

// Remove an item from the production queue.
      case 'r':
      case 'R':
        if (cur_bldg && adjusting_production) {
          int prod_index = i_buildings.get_int("list_benefits");
          if (prod_index >= 0 && prod_index < cur_bldg->build_queue.size()) {
            cur_bldg->build_queue.erase( cur_bldg->build_queue.begin() +
                                         prod_index );
// Keep our index where it is now
            move_index = prod_index;
// Update list_benefits on next loop (i.e. now)
            update_benefits = true;
          }
        }
        break;

// Add an item to the production queue.
      case 'a':
      case 'A':
        if (cur_bldg && adjusting_production) {
          Recipe_amount new_recipe;
          if (pick_recipe(cur_bldg, new_recipe)) {  // Returns false if canceled
            cur_bldg->build_queue.push_back(new_recipe);
// Update list_benefits on next loop (i.e. now)
            update_benefits = true;
          }
        }
        break;

// Move item up in the production queue
      case '<':
        if (cur_bldg && adjusting_production &&
            !cur_bldg->build_queue.empty()) {
          int prod_index = i_buildings.get_int("list_benefits");
// index > 0 since we can't move the first item up any further
          if (prod_index > 0 && prod_index < cur_bldg->build_queue.size()) {
            Recipe_amount tmp = cur_bldg->build_queue[prod_index - 1];
            cur_bldg->build_queue[prod_index - 1] =
              cur_bldg->build_queue[prod_index];
            cur_bldg->build_queue[prod_index] = tmp;
// Set moved_index, so our cursor stays with the item we moved
            move_index = prod_index - 1;
// Refresh the production list
            update_benefits = true;
          }
        }
        break;

// Move item down in the production queue
      case '>':
        if (cur_bldg && adjusting_production &&
            !cur_bldg->build_queue.empty()) {
          int prod_index = i_buildings.get_int("list_benefits");
// index < size() - 1 since we can't move the last item down any further
          if (prod_index >= 0 &&
              prod_index < cur_bldg->build_queue.size() - 1) {
            Recipe_amount tmp = cur_bldg->build_queue[prod_index + 1];
            cur_bldg->build_queue[prod_index + 1] =
              cur_bldg->build_queue[prod_index];
            cur_bldg->build_queue[prod_index] = tmp;
// Set moved_index, so our cursor stays with the item we moved
            move_index = prod_index + 1;
// Refresh the production list
            update_benefits = true;
          }
        }
        break;

      default:
        i_buildings.handle_keypress(ch);
        break;
    }
  } // while (!done)
}

void Interface::build_building()
{
  cuss::interface i_build;
  if (!i_build.load_from_file("cuss/build_building.cuss")) {
    return;
  }

  Window w_build(0, 0, 80, 24);

// Static lists
  std::vector<std::string> options, cost_gold, cost_wood, cost_stone,
                           building_queue, queue_days;

// Dynamic list - the Building_types we're currently browsing.
  std::vector<Building_type> bldg_types;

// First, let's set up our building queue.
  for (int i = 0; i < city->building_queue.size(); i++) {
    building_queue.push_back( city->building_queue[i].get_name() );
    queue_days.push_back( itos( city->building_queue[i].construction_left ) );
  }

// Set up our list and headers to have the building categories
  Building_category build_cat = BUILDCAT_NULL;
  set_building_list(i_build, build_cat, bldg_types);
// Set up the building queue
  set_building_queue(i_build);

  bool done = false;
  bool editing_queue = false;

  while (!done) {
    i_build.draw(&w_build);
    w_build.refresh();

    long ch = input();

// Did we pick an option from our list?
    if (ch >= '1' && ch <= '9') {
      ch -= '0'; // So that '1' => 1, '2' => 2, etc

      if (build_cat == BUILDCAT_NULL) {
        if (ch < BUILDCAT_MAX) {
          build_cat = Building_category(ch);
          set_building_list(i_build, build_cat, bldg_types);
        }

      } else { // Actually building a building, not picking a category
        int index = ch - 1; // Since the list starts at 1 and vector starts at 0
        Building_type btype = bldg_types[index];
        if (index >= 0 && index < bldg_types.size()) {
          Building_queue_status status = city->add_building_to_queue(btype);
  
          switch (status) {
            case BUILDING_QUEUE_OK: // We did it!  Update interface.
  // We need to redo the list in case we are now unable to build something.
              set_building_list(i_build, build_cat, bldg_types);
              set_building_queue(i_build);
              break;
  
            case BUILDING_QUEUE_NO_RESOURCES: {
  // Craft a popup that will tell us why we can't build it
              std::stringstream ss_popup;
              Building_datum* bldg_dat = Building_data[btype];
              ss_popup << "You lack the resources to build that!" << std::endl;
              for (int i = 0; i < bldg_dat->build_costs.size(); i++) {
                ss_popup << std::endl;
                Resource res = bldg_dat->build_costs[i].type;
                int amount   = bldg_dat->build_costs[i].amount;
                int city_amount = city->get_resource_amount(res);
                ss_popup << capitalize( resource_name( res ) ) << ": " <<
                            amount << " (You: ";
                if (city_amount < amount) {
                  ss_popup << "<c=red>";  // This is why we can't build this!
                }
                ss_popup << city_amount << "<c=/>)";
              }
              popup(ss_popup.str().c_str());
            } break;

// TODO: If there are ever any other Building_queue_status values, add them here

          } // switch (status)
        } // if (index >= 0 && index < bldg_types.size())
      } // End of "attempting to build a building" block

    } else { // if (ch >= '1' && ch <= '9')

      switch (ch) {
        case KEY_ESC:
        case 'Q':
        case 'q':
// If we're at the top level, or in the building queue, we leave the screen.
          if (build_cat == BUILDCAT_NULL || editing_queue) {
            done = true;
          } else {  // Return to top level
            build_cat = BUILDCAT_NULL;
            set_building_list(i_build, build_cat, bldg_types);
          }
          break;

        case '\t': { // TAB
          editing_queue = !editing_queue;
          if (editing_queue) { // We moved to control the queue list
            i_build.select("list_building_queue");
          } else {  // Select nothing.
            i_build.select_none();
          }
          int help_count = 0;
          if (build_cat == BUILDCAT_NULL) {
            help_count = BUILDCAT_MAX - 1;
          } else {
            help_count = bldg_types.size();
          }
          set_building_help(i_build, build_cat, editing_queue, help_count);
        } break;

        case '<': // Move queue item up
          if (editing_queue) {
            int index = i_build.get_int("list_building_queue");
// > 0 (not >= 0) since we can't move the first item up.
            if (index > 0 && index < city->building_queue.size()) {
              Building tmp = city->building_queue[index - 1];
              city->building_queue[index - 1] = city->building_queue[index];
              city->building_queue[index] = tmp;
// Update the list
              set_building_queue(i_build);
// Move our cursor along with the building!
              i_build.set_data("list_building_queue", index - 1);
            }
          }
          break;

        case '>': // Move queue item down
          if (editing_queue) {
            int index = i_build.get_int("list_building_queue");
// < size() - 1 (not < size()) since we can't move the last item down.
            if (index >= 0 && index < city->building_queue.size() - 1) {
              Building tmp = city->building_queue[index + 1];
              city->building_queue[index + 1] = city->building_queue[index];
              city->building_queue[index] = tmp;
// Update the list
              set_building_queue(i_build);
// Move our cursor along with the building!
              i_build.set_data("list_building_queue", index + 1);
            }
          }
          break;

        case 'x':
        case 'X':
        case 'd':
        case 'D': // Delete building from queue.
          if (editing_queue) {
            int index = i_build.get_int("list_building_queue");
            if (index >= 0 && index < city->building_queue.size()) {
// Warn the player if this will waste effort
              Building* bldg = &(city->building_queue[index]);
              Building_datum* bldg_dat = bldg->get_building_datum();

              bool do_it = true;
              if (bldg->construction_left < bldg_dat->build_time) {
                int diff = bldg_dat->build_time - bldg->construction_left;
                std::stringstream ss_warning;
                ss_warning << "Really cancel production and waste " << diff <<
                              " days of effort?";
                do_it = query_yn(ss_warning.str().c_str());
              }

              if (do_it) {

                if (!city->cancel_queued_building(index)) { // BAD ERROR
                  debugmsg("city->cancel_queued_building(%d) returned false",
                           index);
                } else {
                  set_building_queue(i_build);
// Attempt to keep the cursor where it was
                  i_build.set_data("list_building_queue", index);
                }

              } // if (do_it)

            } // if (index >= 0 && index < city->building_queue.size())
          } // if (editing_queue)
          break;

        default:
          if (editing_queue) { // Keybindings only work when on queue list
            i_build.handle_keypress(ch);
// Make sure the two lists for the queue are synched
            int index = i_build.get_int("list_building_queue");
            i_build.set_data("list_queue_days", index);
          }
          break;

      } // switch (ch)

    } // if ! (ch >= '1' && ch <= '9')

  } // while (!done)

}

void Interface::set_building_list(cuss::interface& i_build,
                                  Building_category category,
                                  std::vector<Building_type>& types)
{
// We always want to clear list_options, since we're building it here
  i_build.clear_data("list_options");
// Also clear text_description, for safety
  i_build.clear_data("text_description");
// Aaaand clear out types, just in case
  types.clear();

  int help_count = 0; // Used when we cal set_building_help() below

// BUILDCAT_NULL is a special case - it means we want to list the building
// categories, not actual buildings themselves!
  if (category == BUILDCAT_NULL) {
// First, we need to set the header to say "Select a type:", and clear the
// resource header and lists (including the header & numbers for the city)
    i_build.set_data("text_header", "<c=yellow>Select a type:<c=/>");
    i_build.clear_data("text_resource_header" );
    i_build.clear_data("list_cost_gold"       );
    i_build.clear_data("list_cost_wood"       );
    i_build.clear_data("list_cost_stone"      );
// Now, fill list_options with all the names of the different Building_categorys
    for (int i = 1; i < BUILDCAT_MAX; i++) { // Start at 1 to skip BUILDCAT_NULL
      Building_category cat = Building_category(i);
      std::stringstream option;
      option << "<c=pink>" << i << "<c=/>: " <<
                capitalize( building_category_name(cat) );
      i_build.add_data("list_options", option.str());
    }
// Append our options with a way to leave the screen
    i_build.add_data("list_options", "<c=pink>Q<c=/>: Leave screen");

    help_count = BUILDCAT_MAX - 1;

  } else {  // if (category == BUILDCAT_NULL)

// First, set the headers.
    std::stringstream header;
    header << "<c=yellow>" << capitalize( building_category_name(category) ) <<
              "<c=/>";
    i_build.set_data("text_header", header.str());
    i_build.set_data("text_resource_header", "Gold:  Wood:  Stone:");
// Clear out the old cost lists
    i_build.clear_data("list_cost_gold"       );
    i_build.clear_data("list_cost_wood"       );
    i_build.clear_data("list_cost_stone"      );
// Find all the buildings that match our given category and put them in types
    for (int i = 0; i < BUILD_MAX; i++) {
      Building_datum* bldg_dat = Building_data[i];
      if (bldg_dat->category == category) { // It's a match!
        types.push_back( Building_type(i) );
      }
    }
// Now use all the buildings in types to fill out the interface's lists
    for (int i = 0; i < types.size(); i++) {
      Building_type btype = types[i];
      Building_datum* bldg_dat = Building_data[btype];
      std::stringstream ss_name, ss_gold, ss_wood, ss_stone;
// Check whether we can actually build the building.
      bool can_build = true;  // We'll set to false if we lack any resources
      for (int n = 0; n < bldg_dat->build_costs.size(); n++) {
        Resource res = bldg_dat->build_costs[n].type;
        int amount   = bldg_dat->build_costs[n].amount;
        if (city->get_resource_amount(res) < amount) {
          can_build = false;
        }
      }
// Now that we know whether we can build it, go through again & populate lists

/* TODO: We're kind of making an interface-driven design decision that buildings
 *       will ONLY use these three resources in their build_costs.  We should
 *       probably either replace Building_datum::build_costs with a trio of
 *       ints (gold, wood, and stone), or not make this assumption.  For now
 *       we'll have ugly code here, leaving the question unanswered, but
 *       eventually this should be refactored.
 */

// We track whether we added lines for each resource, so we can add a blank line
// later on (otherwise we'll put the next number in the wrong place)
      bool did_gold = false, did_wood = false, did_stone = false;

      for (int n = 0; n < bldg_dat->build_costs.size(); n++) {
        Resource res = bldg_dat->build_costs[n].type;
        int amount   = bldg_dat->build_costs[n].amount;
        std::stringstream* ss_resource = NULL;
        std::string list_name;
        switch (res) {
          case RES_GOLD:
            ss_resource = &ss_gold;
            list_name = "list_cost_gold";
            did_gold = true;
            break;
          case RES_WOOD:
            ss_resource = &ss_wood;
            list_name = "list_cost_wood";
            did_wood = true;
            break;
          case RES_STONE:
            ss_resource = &ss_stone;
            list_name = "list_cost_stone";
            did_stone = true;
            break;
        }
        if (ss_resource) {
          if (city->get_resource_amount(res) < amount) {
// Flag it red so the player knows this is why we can't have this nice thing
            (*ss_resource) << "<c=red>";
          } else if (!can_build) {
// Not the culprit, but still color it dark gray since we can't build this
            (*ss_resource) << "<c=dkgray>";
          }
          (*ss_resource) << amount << "<c=/>";
          i_build.add_data(list_name, ss_resource->str());
        } // if (ss_resource)
      } // for (int n = 0; n < bldg_dat->build_costs.size(); n++)

// Put in a blank line for the lists where we didn't put in a number
      if (!did_gold) {
        i_build.add_data("list_cost_gold", "");
      }
      if (!did_wood) {
        i_build.add_data("list_cost_wood", "");
      }
      if (!did_stone) {
        i_build.add_data("list_cost_stone", "");
      }

// Now we can add the name of the building.
// Use i + 1 since we want our list to start counting from 1, not 0
      ss_name << "<c=pink>" << i + 1 << "<c=/>: ";
      if (!can_build) {
        ss_name << "<c=dkgray>";
      }
      ss_name << capitalize( bldg_dat->name ) << "<c=/>";
      i_build.add_data("list_options", ss_name.str());
    } // for (int i = 0; i < types.size(); i++)

// Append an option to go back at the top of list_options
    i_build.add_data("list_options", "<c=pink>Q<c=/>: Go back");
// Append a blank line, then info on what the player has
    i_build.add_data("list_options", "");
    i_build.add_data("list_options", "<c=white>You have:<c=/>");
// Append two blank lines to the resource columns, then the city's resources
    i_build.add_data("list_cost_gold",  "");
    i_build.add_data("list_cost_gold",  "");
    std::stringstream city_gold;
    city_gold << "<c=white>" << city->get_resource_amount(RES_GOLD) <<
                 "<c=/>";
    i_build.add_data("list_cost_gold", city_gold.str());

    i_build.add_data("list_cost_wood",  "");
    i_build.add_data("list_cost_wood",  "");
    std::stringstream city_wood;
    city_wood << "<c=white>" << city->get_resource_amount(RES_WOOD) <<
                 "<c=/>";
    i_build.add_data("list_cost_wood", city_wood.str());

    i_build.add_data("list_cost_stone", "");
    i_build.add_data("list_cost_stone", "");
    std::stringstream city_stone;
    city_stone << "<c=white>" << city->get_resource_amount(RES_STONE) <<
                 "<c=/>";
    i_build.add_data("list_cost_stone", city_stone.str());

    help_count = types.size();

  } // if (category != BUILDCAT_NULL)

// Finally, set some useful help text.
    set_building_help(i_build, category, false, help_count);
}

void Interface::set_building_queue(cuss::interface& i_build)
{
// Clear the two fields we're about to set up
  i_build.clear_data("list_building_queue");
  i_build.clear_data("list_queue_days"    );
  i_build.clear_data("list_total_days"    );

// Special case if no buildings enqueued
  if (city->building_queue.empty()) {
    i_build.add_data("list_building_queue", "<c=dkgray>Nothing queued<c=/>");
    i_build.add_data("list_queue_days", "<c=dkgray>--<c=/>");
    i_build.add_data("list_total_days", "<c=dkgray>--<c=/>");
    return;
  }

// Now set them up.
  int running_total = 0;
  for (int i = 0; i < city->building_queue.size(); i++) {
    Building* bldg = &(city->building_queue[i]);
    running_total += bldg->construction_left;
    i_build.add_data("list_building_queue", bldg->get_name());
    i_build.add_data("list_queue_days", itos( bldg->construction_left ) );
    i_build.add_data("list_total_days", itos( running_total ) );
  }
}

void Interface::set_building_help(cuss::interface& i_build,
                                  Building_category build_cat,
                                  bool editing_queue, int num_options)
{
  if (editing_queue) {  // Help refers to editing the building queue

    i_build.set_data("text_help", "\
Use direction keys to select a queued building.\n\
<c=pink><<c=/>: Move the selected building up.\n\
<c=pink>><c=/>: Move the selected building down.\n\
<c=pink>D<c=/>: Delete the selected building.\n\
<c=pink>TAB<c=/>: Return to selecting buildings.\n\
<c=pink>Esc<c=/>/<c=pink>Q<c=/>: Leave this screen.\
");

  } else if (build_cat == BUILDCAT_NULL) { // Help refers to picking a category

    std::stringstream help_text;
    help_text << "<c=pink>1<c=/> - <c=pink>" << num_options << "<c=/>: View " <<
                 "buildings in category." << std::endl;
    help_text << "<c=pink>TAB<c=/>: Switch to the building queue (allow you " <<
                 "to change the order or remove items)." << std::endl;
    help_text << "<c=pink>ESC<c=/>/<c=pink>Q<c=/>: Leave this screen.";

    i_build.set_data("text_help", help_text.str());

  } else {  // Help refers to picking a building

    std::stringstream help_text;
    help_text << "<c=pink>1<c=/> - <c=pink>" << num_options << "<c=/>: " <<
                 "Select a building to add to your queue." << std::endl;
    help_text << "<c=pink>TAB<c=/>: Switch to the building queue (allow you " <<
                 "to change the order or remove items)." << std::endl;
    help_text << "<c=pink>ESC<c=/>/<c=pink>Q<c=/>: Pick a new category.";

    i_build.set_data("text_help", help_text.str());

  }
}
    

bool Interface::pick_recipe(Building* cur_bldg, Recipe_amount& new_recipe)
{
  if (!cur_bldg) {
    return false;
  }
// Set up an interface for picking a resource to produce
  cuss::interface i_production;
  if (!i_production.load_from_file("cuss/building_production.cuss")) {
    return false;
  }
  Window w_production(0, 0, 80, 24);

// List of options, units produced per day, the number we already have stored,
// and the components used to create it.
  std::vector<std::string> prod_options, prod_units, prod_stored,
                           prod_components;
  Building_datum* bldg_dat = cur_bldg->get_building_datum();
  for (int i = 0; i < bldg_dat->recipes.size(); i++) {
    Recipe rec = bldg_dat->recipes[i];
// Capitalize and color the name of the resource
    std::stringstream ss_name, ss_units, ss_stored, ss_components;
    ss_name << "<c=yellow>" <<
               capitalize( resource_name( rec.result.type ) ) <<
               " x " << rec.result.amount << "<c=/>";
// Fetch the units per day, converting from days per unit if necessary
    if (rec.units_per_day != 0) {
      ss_units << rec.units_per_day;
    } else if (rec.days_per_unit != 0) {
      ss_units << "1/" << rec.days_per_unit;
    } else { // Units = 0 ?!
      ss_units << "<c=red>0<c=/>";
    }
// Fetch the units stored
    int stored = city->get_resource_amount( rec.result.type );
    if (stored == 0) {
      ss_stored << "<c=dkgray>";
    }
    ss_stored << stored << "<c=/>";
// Fetch the components
    if (rec.resource_ingredients.empty() &&
        rec.mineral_ingredients.empty()) {
      prod_components.push_back( std::string() );
    } else {
      for (int n = 0; n < rec.resource_ingredients.size(); n++) {
        Resource ing_type = rec.resource_ingredients[n].type;
        int ing_amount    = rec.resource_ingredients[n].amount;
        ss_components << ing_amount << " x " <<
                         capitalize( resource_name( ing_type ) ) <<
                         std::endl;
      }
      for (int n = 0; n < rec.mineral_ingredients.size(); n++) {
        Mineral ing_type = rec.mineral_ingredients[n].type;
        int ing_amount   = rec.mineral_ingredients[n].amount;
        ss_components << ing_amount << " x " <<
                         capitalize( Mineral_data[ing_type]->name ) <<
                         std::endl;
      }
    }
// Now put the contents of the stringstreams where they belong.
    prod_options.push_back    ( ss_name.str()       );
    prod_units.push_back      ( ss_units.str()      );
    prod_stored.push_back     ( ss_stored.str()     );
    prod_components.push_back ( ss_components.str() );
  } // for (int i = 0; i < bldg_dat->recipes.size(); i++) 

// Set up our interface.  We don't use prod_components here, since we
// only display components of the currently-selected recipe.
  i_production.set_data("list_options", prod_options);
  i_production.set_data("list_units",   prod_units  );
  i_production.set_data("list_stored",  prod_stored );

// Amount defaults to 1.
  int amount = 1;
  i_production.set_data("text_amount", "1");

// Start by selecting list_options
  i_production.select("list_options");

// Now actually run the interface!
  while (true) {
    int index = i_production.get_int("list_options");
    if (index >= 0 && index < bldg_dat->recipes.size()) {
// NOW we use prod_components.
      i_production.set_data("text_components", prod_components[index]);
    }
    i_production.draw( &w_production );
    w_production.refresh();

    long ch = input();

    switch (ch) {

      case '0':
        amount = 0;
        i_production.set_data("text_amount", "<c=ltblue>Infinite<c=/>");
        break;

      case '+':
      case '=':
      case 'l':
      case '6':
      case KEY_RIGHT:
        amount++;
        i_production.set_data("text_amount", itos(amount));
        break;

      case '-':
      case 'h':
      case '4':
      case KEY_LEFT:
        if (amount > 1) {
          amount--;
          i_production.set_data("text_amount", itos(amount));
        }
        break;

      case '\n':
        if (index >= 0 && index < bldg_dat->recipes.size()) {
          new_recipe.recipe = bldg_dat->recipes[index];
          new_recipe.amount = amount;
          if (new_recipe.amount == 0) {
            new_recipe.amount = INFINITE_RESOURCE;
          }
          return true;
        }
        break;

      case 'q':
      case 'Q':
      case KEY_ESC:
        return false;

      default:
        i_production.handle_keypress(ch);
        break;
    }
  }
}

Area_type Interface::pick_area()
{
  i_main.clear_data("text_map_info");
  i_main.clear_data("text_commands");

  Area_category cur_category = AREACAT_NULL;
  std::vector<Area_type> types;

  set_area_list(cur_category, types);

  while (true) {

    i_main.draw(&w_main);
    w_main.refresh();

    long ch = input();

    if (ch >= '1' && ch <= '9') { // We made a selection!
      ch -= '1';  // '1' => 0, '2' => 1, etc.
      if (cur_category == AREACAT_NULL) { // We selected a category.
        ch++; // Sinc ewe don't list AREACAT_NULL.
        if (ch < AREACAT_MAX) {
          cur_category = Area_category(ch);
          set_area_list(cur_category, types);
        }

      } else { // We selected an area.
        if (ch < types.size()) {
          i_main.clear_data("text_commands");
          return types[ch];
        }
      }
    } else if (ch == KEY_ESC || ch == 'q' || ch == 'Q') {
// The effect depends on whether we've already selected a category.
      if (cur_category == AREACAT_NULL) {
        i_main.clear_data("text_commands");
        return AREA_NULL;
      } else {
        cur_category = AREACAT_NULL;
        set_area_list(cur_category, types);
      }
    }
  } // while (true)
}

void Interface::set_area_list(Area_category category,
                              std::vector<Area_type>& types)
{
  i_main.clear_data("text_commands");
  types.clear();

  if (category == AREACAT_NULL) {
// Start at 1 to skill AREACAT_NULL
    for (int i = 1; i < AREACAT_MAX; i++) {
      Area_category cat = Area_category(i);
      std::stringstream ss_cat;
      ss_cat << "<c=pink>" << i << "<c=/>: " <<
                capitalize( area_category_name( cat ) ) << std::endl;
      i_main.add_data("text_commands", ss_cat.str());
    }
// Add an option to cancel.
    i_main.add_data("text_commands", "<c=pink>Q<c=/>: Cancel");
    return;

  } else {  // Actually listing areas.

// First, find all areas that belong to our chosen category.
    for (int i = 0; i < AREA_MAX; i++) {
      Area_datum* area_dat = Area_data[i];
      if (area_dat->category == category) {
        types.push_back( Area_type(i) );
      }
    }
// Next, display them in our interface.
    for (int i = 0; i < types.size(); i++) {
      std::stringstream ss_area;
      Area_datum* area_dat = Area_data[ types[i] ];
      ss_area << "<c=pink>" << i + 1 << "<c=/>: " <<
                 capitalize( area_dat->name ) << std::endl;
      i_main.add_data("text_commands", ss_area.str());
    }
// Finally, add an option to go back to category selection.
    i_main.add_data("text_commands", "<c=pink>Q<c=/>: Change category");

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
