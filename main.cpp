#include "city.h"
#include "rng.h"
#include "window.h"

int main()
{
  srand(time(NULL));

  init_display();

  City city;

  endwin();
  return 0;
}

