#include "keys.h"

Point input_direction(long ch)
{
  switch (ch) {
    case 'k': return Point( 0, -1);
    case 'u': return Point( 1, -1);
    case 'l': return Point( 1,  0);
    case 'n': return Point( 1,  1);
    case 'j': return Point( 0,  1);
    case 'b': return Point(-1,  1);
    case 'h': return Point(-1,  0);
    case 'y': return Point(-1, -1);
    case '.': return Point( 0,  0);
    default:              return Point(-2, -2); // Be sure to check for this!
  }
  return Point(-2, -2);
}
