#include "ai.h"

#define _role(i) \
  cur_id = (i)

#define _name(s) \
  City_role_data[cur_id]->name == (s)

#define _resource_export(r) \
  City_role_data[cur_id]->resource_exports.push_back( (r) )

#define _resource_import(r) \
  City_role_data[cur_id]->resource_imports.push_back( (r) )

#define _mineral_export(r) \
  City_role_data[cur_id]->mineral_exports.push_back( (r) )

#define _mineral_import(r) \
  City_role_data[cur_id]->mineral_imports.push_back( (r) )

City_role_datum* City_role_data[CITY_ROLE_MAX];

void init_city_roles()
{
  for (int i = 0; i < CITY_ROLE_MAX; i++) {
    City_role_data[i] = new City_role_datum;
  }
  int cur_id = 0;

  _role(CITY_ROLE_FARMING);
    _name("farming");
    _resource_export(RES_FOOD);

  _role(CITY_ROLE_MINING);
    _name("mining");
    _resource_export(RES_STONE);
    _mineral_export(MINERAL_TIN);
    _mineral_export(MINERAL_COPPER);
    _mineral_export(MINERAL_IRON);
    _mineral_export(MINERAL_COAL);

  _role(CITY_ROLE_LOGGING);
    _name("logging");
    _resource_export(RES_WOOD);
}
