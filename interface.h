#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include "cuss.h"
#include "window.h"
#include "area.h"
#include <vector>
#include <string>

enum Menu_id
{
  MENU_NULL = 0,
  MENU_GAME,
  MENU_MINISTERS,
  MENU_BUILD,
  MENU_MAX
};

enum Interface_mode
{
  IMODE_NULL = 0,
  IMODE_MENU,
  IMODE_VIEW_MAP,
  IMODE_MAX
};

enum Game_state
{
  GAME_NORMAL,
  GAME_QUIT
};

class City;

struct Command
{
  std::string name;
  int index;
};

struct Menu
{
  std::string name;
  Menu_id id;
  int posx;
  std::vector<std::string> items;
};

// Basically, highlights the first letter.
std::string menuify(std::string name);

class Interface
{
public:
  Interface();
  ~Interface();

  bool init(City* C);

  void main_loop();

private:
/* This is a modal interface, so keys are handled differently depending on what
 * mode we're in.  Esc and ! are unique; Esc always sets mode/menu to NULL, and
 * ! always sets mode to Menu and menu to NULL.
 */
  void handle_key(long ch);
// Modes
  void set_mode(Interface_mode mode);
// Menus
  void set_menu(Menu_id item);
  void do_menu_action(Menu_id menu, int index);

// Action functions
  void enqueue_area();

// Special screens
  void minister_finance();
  Area_type pick_area();

// Helper / data-storing functions
  void get_menu_info(Menu_id item, std::string& name, int& posx);
  std::vector<std::string> get_menu_options(Menu_id item);

// Setup functions
  bool add_menu(Menu_id id, std::string name, ...);
  void set_menu_str();

// DATA
  Menu_id cur_menu;
  std::vector<Menu> menus;
  std::string menu_str;
  int next_menu_posx;

  Game_state game_state;
  Interface_mode cur_mode;

  City* city;
  Point sel;
  bool city_radius;
  Area_type current_area;

  cuss::interface i_main;
  Window w_main;
  cuss::interface i_menu;
  Window w_menu;

};

#endif
