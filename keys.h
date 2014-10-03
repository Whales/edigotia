#ifndef _KEYBIND_H_
#define _KEYBIND_H_

#include "geometry.h"

// Returns "one step in direction bound to ch"
Point input_direction(long ch, bool accept_capitals = false);

#endif
