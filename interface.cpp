#include "interface.h"
#include "window.h"
#include "cuss.h"
#include "city.h"

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

  return true;
}

void Interface::main_loop()
{
  city->draw_map(i_main.find_by_name("draw_map"));

  i_main.draw(&w_main);
  w_main.refresh();

  bool done = false;
  while (!done) {
    long ch = input();
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

void Interface::toggle_menu(cuss::interface* i_menu_bar, Menu_item item)
{
  if (!i_menu_bar) {
    return;
  }

  std::string menu_name;
  switch (item) {
    case MENU_CITY:
      menu_name = "list_city";
      break;
  }
  if (cur_menu == item) {
    cur_menu = MENU_NULL;
    i_menu_bar->clear_data(menu_name);
  } else {
    cur_menu = item;
    i_menu_bar->set_data(menu_name, get_menu_options(item));
  }
}

std::vector<std::string> Interface::get_menu_options(Menu_item item)
{
  std::vector<std::string> ret;

  switch (item) {
    case MENU_CITY:
      ret.push_back("<c=ltred>V<c=/>iew");
      break;
  }

  return ret;
}
