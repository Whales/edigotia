#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include "cuss.h"
#include "window.h"
#include <vector>
#include <string>

enum Menu_item
{
  MENU_NULL = 0,
  MENU_MINISTERS,
  MENU_MAX
};

enum Interface_mode
{
  IMODE_NULL = 0,
  IMODE_CITY,
  IMODE_MAX
};

class City;

// Basically, highlights the first letter.
std::string menuify(std::string name);

struct Interface
{
  Interface();
  ~Interface();

  bool init(City* C);

  void main_loop();

// Modes
  void set_mode(Interface_mode mode);
// Menus
  void set_menu(Menu_item item);

// DATA
  Menu_item cur_menu;
  Interface_mode cur_mode;
  City* city;

  cuss::interface i_main;
  Window w_main;
  cuss::interface i_menu;
  Window w_menu;

private:
  void get_menu_info(Menu_item item, std::string& name, int& posx);
  std::vector<std::string> get_menu_options(Menu_item item);
};

#endif
