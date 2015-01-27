#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <string>

class Game;

extern Game* GAME;
extern std::string SAVE_DIR;

bool set_directories();

#endif
