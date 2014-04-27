#include "area.h"

Area_datum* Area_data[AREA_MAX];

#define _area(x) \
  cur_id = (x)
#define _name(n) \
  Area_data[cur_id]->name = (n)
#define _symbol(s, f) \
  Area_data[cur_id]->symbol = glyph(s, f, c_black)
#define _building(bt) \
  Area_data[cur_id]->building = (bt)

void init_area_data()
{
  for (int i = 0; i < AREA_MAX; i++) {
    Area_data[i] = new Area_datum;
    Area_data[i]->uid = i;
  }
  int cur_id = 0;

  _area(AREA_HOVELS);
    _name("Hovels");
    _symbol('o', c_brown);
    _building(BUILD_HOVEL);

  _area(AREA_HOUSES);
    _name("Houses");
    _symbol('o', c_ltgreen);
    _building(BUILD_HOUSES);

  _area(AREA_MANOR);
    _name("manor");
    _symbol('o', c_ltblue);
    _building(BUILD_MANOR);

  _area(AREA_PALACE);
    _name("palace");
    _symbol('@', c_yellow);
    _building(BUILD_PALACE);

  _area(AREA_FARM);
    _name("farm");
    _symbol('#', c_ltgreen);
    _building(BUILD_FARM);

  _area(AREA_QUARRY);
    _name("quarry");
    _symbol('Q', c_ltgray);
    _building(BUILD_QUARRY);

  _area(AREA_MINE);
    _name("mine");
    _symbol('M', c_brown);
    _building(BUILD_MINE);

  _area(AREA_BARRACKS);
    _name("barracks");
    _symbol('B', c_red);
    _building(BUILD_BARRACKS);
}
