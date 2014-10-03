#include "keys.h"
#include <ncurses.h>  // for KEY_ macros

Point input_direction(long ch, bool accept_capitals)
{
// accept_capitals defaults to false
  if (accept_capitals) {
    switch (ch) {
      case 'K': return Point( 0, -1);
      case 'U': return Point( 1, -1);
      case 'L': return Point( 1,  0);
      case 'N': return Point( 1,  1);
      case 'J': return Point( 0,  1);
      case 'B': return Point(-1,  1);
      case 'H': return Point(-1,  0);
      case 'Y': return Point(-1, -1);
    }
  }

  switch (ch) {
    case 'k':       return Point( 0, -1);
    case 'u':       return Point( 1, -1);
    case 'l':       return Point( 1,  0);
    case 'n':       return Point( 1,  1);
    case 'j':       return Point( 0,  1);
    case 'b':       return Point(-1,  1);
    case 'h':       return Point(-1,  0);
    case 'y':       return Point(-1, -1);
    case KEY_UP:    return Point( 0, -1);
    case KEY_RIGHT: return Point( 1,  0);
    case KEY_DOWN:  return Point( 0,  1);
    case KEY_LEFT:  return Point(-1,  0);
    case '1':       return Point(-1,  1);
    case '2':       return Point( 0,  1);
    case '3':       return Point( 1,  1);
    case '4':       return Point(-1,  0);
    case '6':       return Point( 1,  0);
    case '7':       return Point(-1, -1);
    case '8':       return Point( 0, -1);
    case '9':       return Point( 1, -1);
    case '5': // Fall through
    case '.':       return Point( 0,  0);
    default:        return Point(-2, -2); // Be sure to check for this!
  }
  
  return Point(-2, -2);
}
