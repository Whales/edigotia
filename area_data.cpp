#include "area.h"

Area_datum* Area_data[AREA_MAX];

#define _area(x) \
  cur_id = (x)

#define _name(n) \
  Area_data[cur_id]->name = (n)

#define _symbol(s, f) \
  Area_data[cur_id]->symbol = glyph((s), (f), c_black)

#define _category(c) \
  Area_data[cur_id]->category = (c)

#define _building(bt) \
  Area_data[cur_id]->building = (bt)

#define _buildings_supported(n) \
  Area_data[cur_id]->buildings_supported = (n)

#define _unlock(t, a, b) \
  Area_data[cur_id]->unlockable = true; \
  Area_data[cur_id]->unlock_condition = City_achievement( (t), (a), (b) )

void init_area_data()
{
  for (int i = 0; i < AREA_MAX; i++) {
    Area_data[i] = new Area_datum;
    Area_data[i]->uid = i;
  }
  int cur_id = 0;

  _area(AREA_PARK);
    _name("park");
    _symbol('P', c_ltgreen);
    _category(AREACAT_INFRASTRUCTURE);
    _building(BUILD_PARK);

  _area(AREA_PLAZA);
    _name("plaza");
    _symbol('&', c_ltgreen);
    _category(AREACAT_INFRASTRUCTURE);
    _building(BUILD_PLAZA);
    _buildings_supported(4);

  _area(AREA_MARKETPLACE);
    _name("marketplace");
    _symbol('&', c_ltblue);
    _category(AREACAT_INFRASTRUCTURE);
    _building(BUILD_MARKETPLACE);
    _buildings_supported(10);

  _area(AREA_HOVELS);
    _name("hovels");
    _symbol('o', c_yellow);
    _category(AREACAT_HOUSING);
    _building(BUILD_HOVEL);

  _area(AREA_HOUSES);
    _name("houses");
    _symbol('O', c_ltgreen);
    _category(AREACAT_HOUSING);
    _building(BUILD_HOUSE);
    _unlock(CITY_ACHIEVE_POP, CIT_MERCHANT, 1);

  _area(AREA_MANOR);
    _name("manor");
    _symbol('Q', c_blue);
    _category(AREACAT_HOUSING);
    _building(BUILD_MANOR);
    _unlock(CITY_ACHIEVE_POP, CIT_BURGHER,  1);

  _area(AREA_KEEP);
    _name("keep");
    _symbol('@', c_yellow);
    _category(AREACAT_HOUSING);
    _building(BUILD_KEEP);
    _buildings_supported(6);

  _area(AREA_FARM);
    _name("farm");
    _symbol('F', c_ltgreen);
    _category(AREACAT_FOOD);
    _building(BUILD_FARM);

  _area(AREA_HUNTING_CAMP);
    _name("hunting camp");
    _symbol('H', c_ltred);
    _category(AREACAT_FOOD);
    _building(BUILD_HUNTING_CAMP);

  _area(AREA_MINE);
    _name("mine");
    _symbol('M', c_yellow);
    _category(AREACAT_RESOURCES);
    _building(BUILD_MINE);

  _area(AREA_SAWMILL);
    _name("sawmill");
    _symbol('S', c_brown);
    _category(AREACAT_RESOURCES);
    _building(BUILD_SAWMILL);

  _area(AREA_PASTURE);
    _name("pasture");
    _symbol('0', c_brown);
    _category(AREACAT_FOOD);
    _building(BUILD_PASTURE);

  _area(AREA_BARRACKS);
    _name("barracks");
    _symbol('B', c_red);
    _category(AREACAT_MILITARY);
    _building(BUILD_BARRACKS);
}
