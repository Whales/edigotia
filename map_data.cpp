#include "map.h"

Terrain_datum* Terrain_data[TER_MAX];

#define _ter(n) \
  cur_id = n
#define _name(n) \
  Terrain_data[cur_id]->name = n
#define _symbol(c, f) \
  Terrain_data[cur_id]->symbol = glyph(c, f, c_black)
#define _farm(n) \
  Terrain_data[cur_id]->farm_percent = n

void init_terrain_data()
{
  for (int i = 0; i < TER_MAX; i++) {
    Terrain_data[i] = new Terrain_datum;
  }
  int cur_id = 0;

  _ter(TER_NULL);
    _name("NULL");
    _symbol('x', c_red);

  _ter(TER_TUNDRA);
    _name("tundra");
    _symbol('*', c_white);
    _farm(10);

  _ter(TER_FIELD);
    _name("Field");
    _symbol('.', c_green);
    _farm(100);

  _ter(TER_ROCKY);
    _name("Rocky");
    _symbol(',', c_brown);
    _farm(70);

  _ter(TER_HILL);
    _name("Hill");
    _symbol('n', c_brown);
    _farm(50);

  _ter(TER_MOUNTAIN);
    _name("Mountain");
    _symbol('M', c_ltgray);
    _farm(5);

  _ter(TER_FOREST);
    _name("Forest");
    _symbol('7', c_ltgreen);
    _farm(10);

  _ter(TER_RIVER);
    _name("River");
    _symbol('#', c_blue);
    _farm(0);

  _ter(TER_OCEAN);
    _name("Ocean");
    _symbol('#', c_ltblue);
    _farm(0);

  _ter(TER_DESERT);
    _name("Desert");
    _symbol('x', c_yellow);
    _farm(15);

  _ter(TER_SWAMP);
    _name("Swamp");
    _symbol('s', c_cyan);
    _farm(50);
}