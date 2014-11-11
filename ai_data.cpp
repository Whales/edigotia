#include "ai.h"

#define _spec(i) \
  cur_id = (i)

#define _name(s) \
  City_spec_data[cur_id]->name == (s)

#define _res_export(r) \
  City_spec_data[cur_id]->res_exports.push_back( (r) )

#define _res_import(r) \
  City_spec_data[cur_id]->res_imports.push_back( (r) )

#define _min_export(r) \
  City_spec_data[cur_id]->min_exports.push_back( (r) )

#define _min_import(r) \
  City_spec_data[cur_id]->min_imports.push_back( (r) )

void init_city_specializations()
{
  for (int i = 0; i < CITYSPEC_MAX; i++) {
    City_spec_data[i] = new City_spec_datum;
  }
  int cur_id = 0;

  _spec(CITYSPEC_FARMING);
    _name("farming");
    _res_export(RES_FOOD);

  _spec(CITYSPEC_MINING);
    _name("mining");
    _res_export(RES_STONE);
    _min_export(MINERAL_TIN);
    _min_export(MINERAL_COPPER);
    _min_export(MINERAL_IRON);
    _min_export(MINERAL_COAL);
}
