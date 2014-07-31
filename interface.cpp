#include "interface.h"
#include "window.h"
#include "cuss.h"
#include "city.h"
#include "stringfunc.h"
#include <sstream>

Interface::Interface()
{
  cur_menu = MENU_NULL;
}

Interface::~Interface()
{
}

bool Interface::init(City* C)
{
  if (!C) {
    debugmsg("Interface init'd with NULL city.");
    return false;
  }

  city = C;

  if (!i_main.load_from_file("cuss/interface.cuss")) {
    debugmsg("Failed to load critical interface file cuss/interface.cuss!");
    return false;
  }

  w_main.init(0, 0, 80, 24);

/*
  if (!i_menu.load_from_file("cuss/menu_bar.cuss")) {
    debugmsg("Failed to load critical interface file cuss/menu_bar.cuss!");
    return false;
  }

  w_menu.init(0, 0, 80, 24);
*/

  return true;
}

void Interface::main_loop()
{
  city->draw_map(i_main.find_by_name("draw_map"));


  bool done = false;
  while (!done) {
    i_main.draw(&w_main);
    w_main.refresh();
    long ch = input();

    if (ch == KEY_ESC) {
      set_menu(MENU_NULL);

    } else if (ch == 'm' || ch == 'M') {
      set_menu(MENU_MINISTERS);
    }
    

  }
}

/*
void Interface::main_loop()
{
  Window w_menu_bar(0, 0, 80, 24);
  cuss::interface i_menu_bar;
  if (!i_menu_bar.load_from_file("cuss/menu_bar.cuss")) {
    return;
  }

  bool done = false;
  while (!done) {
    i_menu_bar.draw(&w_menu_bar);
    w_menu_bar.refresh();

    long ch = getch();
    if (ch == KEY_ESC) {
      toggle_menu(&i_menu_bar, cur_menu);
    } else if (ch == '.') {
      city->do_turn();
    } else if (ch == '>') {
      for (int i = 0; i < 7; i++) {
        city->do_turn();
      }
    }
    switch (cur_menu) {

      case MENU_NULL:
        switch (ch) {
          case 'c':
          case 'C':
            toggle_menu(&i_menu_bar, MENU_CITY);
          break;
        }
        break;

      case MENU_CITY:
        switch (ch) {
          case 'c':
          case 'C':
            toggle_menu(&i_menu_bar, MENU_CITY);
          break;
          case 'v':
          case 'V':
            city->draw_map(NULL, NULL, true, true);
            break;
        }
        break;
    }
  }
}
*/

void Interface::set_menu(Menu_item item)
{
  std::string menu_name;
  int posx = -1;
// If we're setting the currently-open menu, instead just close it.
  if (item == cur_menu) {
    item = MENU_NULL;
  }
// Remove any currently-open menu.
  if (cur_menu != MENU_NULL) {
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

  i_main.set_data(menu_name, menu_items);
  i_main.set_selectable(menu_name, false);

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

void Interface::get_menu_info(Menu_item item, std::string& name, int& posx)
{
  switch (item) {
    case MENU_MINISTERS:
      name = "list_ministers";
      posx = 2;
      break;
  }
}

std::vector<std::string> Interface::get_menu_options(Menu_item item)
{
  std::vector<std::string> ret;

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

std::string menuify(std::string name)
{
  if (name.empty()) {
    return std::string();
  }
  std::stringstream ret;
  ret << "<c=ltred>" << name[0] << "<c=/>" << name.substr(1);
  return ret.str();
}
