#include "map.h"

Map_type_datum* Map_type_data[MAP_MAX];

#define _ter(n) \
  cur_id = n
#define _name(n) \
  Map_type_data[cur_id]->name = n
#define _symbol(c, f) \
  Map_type_data[cur_id]->symbol = glyph(c, f, c_black)

void init_map_type_data()
{
  int cur_id = 0;
  for (int i = 0; i < MAP_MAX; i++) {
    Map_type_data[i] = new Map_type_datum;
  }

  _ter(MAP_TUNDRA);
    _name("tundra");
    _symbol('*', c_white);

  _ter(MAP_DESERT);
    _name("desert");
    _symbol('x', c_yellow);

  _ter(MAP_WASTELAND);
    _name("wasteland");
    _symbol(',', c_brown);

  _ter(MAP_PLAINS);
    _name("plains");
    _symbol('.', c_green);

  _ter(MAP_FOREST);
    _name("forest");
    _symbol('7', c_ltgreen);

  _ter(MAP_SWAMP);
    _name("swamp");
    _symbol('s', c_cyan);

  _ter(MAP_FOOTHILLS);
    _name("foothills");
    _symbol('n', c_brown);

  _ter(MAP_MOUNTAINOUS);
    _name("mountain range");
    _symbol('M', c_ltgray);

  _ter(MAP_BASIN);
    _name("river basin");
    _symbol('S', c_blue);

  _ter(MAP_CANYON);
    _name("canyon");
    _symbol('S', c_ltblue);

  _ter(MAP_COASTAL);
    _name("coastal");
    _symbol('.', c_yellow);

  _ter(MAP_OCEAN);
    _name("ocean");
    _symbol('=', c_blue);

  _ter(MAP_ICECAP);
    _name("icecap");
    _symbol('=', c_ltcyan);
}
