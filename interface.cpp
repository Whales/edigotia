#include "interface.h"
#include "window.h"
#include "cuss.h"
#include "city.h"
#include "stringfunc.h"
#include <sstream>
#include <cstdarg> // For the variadic function below

Interface::Interface()
{
  cur_menu = MENU_NULL;
  cur_mode = IMODE_NULL;
  next_menu_posx = 2;
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
  i_main.set_data("text_menu_bar", menu_str);

  bool done = false;
  while (!done) {
    i_main.draw(&w_main);
    w_main.refresh();
    long ch = input();

    if (ch == KEY_ESC) {
      set_mode(IMODE_NULL);
      set_menu(MENU_NULL);

    } else if (ch == '!') {
      set_mode(IMODE_MENU);
      set_menu(MENU_NULL);

    } else {
      handle_key(ch);
    }
  }
}

void Interface::handle_key(long ch)
{
  if (ch >= '1' && ch <= '9') { // Accessing or using a menu!

/* We start counting at 1 because this is tied to the interface.  All menus and
 * items in menus start counting at 1, so we do too.
 */
    int menu_index = ch - '0';

    if (cur_menu == MENU_NULL) { // We're not in a menu - so open one
      set_menu( Menu_id( menu_index ) );
    } else {
      do_menu_action(cur_menu, menu_index);
    }

  } else {

    switch (cur_mode) {
// TODO: Fill this in with all keybindings for all modes.
    }

  }
}

void Interface::set_mode(Interface_mode mode)
{
  i_main.clear_data("text_info");
  switch (mode) {

    case IMODE_NULL:
    case IMODE_MENU:
      i_main.clear_data("text_commands");
      break;

    case IMODE_VIEW_MAP:
      i_main.set_data("text_commands", "\
Use movement keys to scroll.\n\
<c=pink>Enter<c=/>: Get info on tile\n\
<c=pink>R<c=/>: Toggle control radius");
      break;

    case IMODE_AREAS:
      i_main.set_data("text_commands", "\
<c=pink>B<c=/>uild Area\n\
<c=pink>C<c=/>lose Area\n\
<c=pink>D<c=/>estroy Area");
      break;

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

void Interface::do_menu_action(Menu_id menu, int index)
{
}

void Interface::minister_finance()
{
}

void Interface::get_menu_info(Menu_id item, std::string& name, int& posx)
{
  name = menus[item].name;
  posx = menus[item].posx;
}

std::vector<std::string> Interface::get_menu_options(Menu_id item)
{
  std::vector<std::string> ret;

  for (int i = 0; i < menus[item].items.size(); i++) {
    std::stringstream ss_name;
    ss_name << "<c=pink>" << i + 1 << "<c=/>: " << menus[item].items[i];
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
  if (menu_str.length() < width) {
    menu_str += "<c=blue,blue>";
    while (menu_str.length() < width) {
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
