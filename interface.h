#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include "cuss.h"
#include "window.h"
#include "area.h"
#include "game.h"
#include <vector>
#include <string>

enum Menu_id
{
  MENU_NULL = 0,
  MENU_GAME,
  MENU_MINISTERS,
  MENU_BUILDINGS,
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

class Player_city;

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

  bool init(Game* G, Player_city* C);

  void main_loop();

private:
/* This is a modal interface, so keys are handled differently depending on what
 * mode we're in.  Esc and ! are unique; Esc always sets mode/menu to NULL, and
 * ! always sets mode to Menu and menu to NULL.
 */
  void handle_key(long ch);


// *** Modes ***

// Set the current mode
  void set_mode(Interface_mode mode);


// *** Menus ***

// Open the specified menu; if item == MENU_NULL, close all menus.
  void set_menu(Menu_id item);
// Perform the action bound to the given menu item
  void do_menu_action(Menu_id menu, int index);


// *** Interface adjustment ***

// Sets text_info to the given text and store the previous text
  void set_temp_info(std::string text);
// Reset text_info to the previous text (original_info_text)
  void restore_info_text();
// Displays city stats relevant to the given Area_type.
// For instance, for a farm, display the current food production/consumption
// For houses, display current housing needs
  void display_area_stats(Area_type type);


// *** Game action functions ***

// Attempt to add current_area to the city's build queue
  void enqueue_area();


// *** Special screens ***

// Finance minister, displays information on income/expenditures
  void minister_finance();
// Food minister, define what crops to grow and other food options
  void minister_food();
  // Helper functions for minister_food()
  void list_farm_crops(Area* cur_farm, cuss::interface& i_food);
// Hunt minister, view hunt history and decide what action to take for animals
  void minister_hunt();
// Mine minister, define what minerals to mine and view stats
  void minister_mining();
  // Helper function for minister_mining()
  void list_mine_minerals(Area* cur_mine, std::vector<int>& mineral_indices,
                          cuss::interface& i_mining);
// Happiness minister, view morale information and do things to alter it
  void minister_morale();
// Info on buildings; allows player to hire workers.
  void building_status();
// Pick a recipe for a building to build
// We stick the data in new_recipe; if we cancel, return false, otherwise true
  bool pick_recipe(Building* cur_bldg, Recipe_amount& new_recipe);
// Select a building to add to our city
  void build_building();
  // Helper functions for build_building()
  // Sets up the left side (list of available buildings in this category)
  void set_building_list(cuss::interface& i_build, Building_category category,
                         std::vector<Building_type>& types);
  // Sets up the right side (our current building queue)
  void set_building_queue(cuss::interface& i_build);
  // Sets up the help box depending on circumstances
  void set_building_help(cuss::interface& i_build, Building_category build_cat,
                         bool editing_queue, int num_options);
// Pick an area to build
  Area_type pick_area();
  // Helper functions for pick_area();
  void set_area_list(Area_category category, std::vector<Area_type>& types);


// *** Helper / data-storing functions ***

// Stores the name and position (on menu bar) of the given menu in name & posx
  void get_menu_info(Menu_id item, std::string& name, int& posx);
// Returns the list of options in the specified menu
  std::vector<std::string> get_menu_options(Menu_id item);


// *** Setup functions ***

// Adds a menu to the menu bar.  Returns false if there's no room for it.
// The variadic parameters are a null-terminated set of strings; each one is an
// option in this menu.
  bool add_menu(Menu_id id, std::string name, ...);
// Sets the string to be stored in the menu bar
  void set_menu_str();

// *** DATA ***
// Menu info - all static.
  std::vector<Menu> menus;
  std::string menu_str;
  int next_menu_posx;

// Current mode.
  Game_state game_state; // e.g. "quit"
  Interface_mode cur_mode;
  Menu_id cur_menu;

// Pointers to game data.
  Game* game;
  Player_city* city;

// Varius values used in user interaction.
  Point sel;  // Point in map currently highlighted
  bool city_radius; // If true, gray out map tiles outside of radius of control
  bool show_terrain;// If true, don't draw areas - just the terrain
  Area_type current_area; // Current area to be built
  std::string original_info_text; // Original text of text_info, to be restored

// Windows and cuss interfaces.
  cuss::interface i_main;
  Window w_main;
  cuss::interface i_menu;
  Window w_menu;

};

#endif
