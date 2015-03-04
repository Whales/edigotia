#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <string>

class Game;
class Help_database;

extern Game* GAME;
extern Help_database* HELP;
extern std::string SAVE_DIR;

bool set_directories();

#endif
