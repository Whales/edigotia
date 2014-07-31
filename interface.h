#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include "cuss.h"
#include "window.h"
#include <vector>
#include <string>

enum Menu_item
{
  MENU_NULL = 0,
  MENU_CITY,
  MENU_MAX
};

class City;

struct Interface
{
  Interface();
  ~Interface();

  bool init(City* C);

  void main_loop();

  void toggle_menu(cuss::interface* i_menu_bar, Menu_item item);
  std::vector<std::string> get_menu_options(Menu_item item);

// DATA
  Menu_item cur_menu;
  City* city;

  cuss::interface i_main;
  Window w_main;
};

#endif
