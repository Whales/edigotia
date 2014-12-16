#include "ai.h"

#define _role(i) \
  cur_id = (i)

#define _name(s) \
  City_role_data[cur_id]->name = (s)

#define _skill(s, n) \
  City_role_data[cur_id]->skill = (s); \
  City_role_data[cur_id]->skill_req = (n)

#define _food_percentage(n) \
  City_role_data[cur_id]->food_percentage = (n)

#define _resource_export(r) \
  City_role_data[cur_id]->resource_exports.push_back( (r) )

#define _resource_import(r) \
  City_role_data[cur_id]->resource_imports.push_back( (r) )

#define _mineral_export(r) \
  City_role_data[cur_id]->mineral_exports.push_back( (r) )

#define _mineral_import(r) \
  City_role_data[cur_id]->mineral_imports.push_back( (r) )

City_role_datum* City_role_data[CITY_ROLE_MAX];

City_role_datum::City_role_datum()
{
  name = "Unitialized Role";
  food_percentage = 90; // Most roles require some imports
  skill = SKILL_NULL;
  skill_req = 0;
}

void init_city_roles()
{
  for (int i = 0; i < CITY_ROLE_MAX; i++) {
    City_role_data[i] = new City_role_datum;
  }
  int cur_id = 0;

  _role(CITY_ROLE_NULL);
    _name("none");
    _food_percentage(100);

  _role(CITY_ROLE_FARMING);
    _name("farming");
    _skill(SKILL_FARMING, 3);
    _food_percentage(125);

  _role(CITY_ROLE_HUNTING);
    _name("hunting");
    _skill(SKILL_HUNTING, 2); // We're forgiving!
    _food_percentage(120);

  _role(CITY_ROLE_LIVESTOCK);
    _name("livestock");
    _skill(SKILL_LIVESTOCK, 3);
    _food_percentage(120);

  _role(CITY_ROLE_MINING);
    _name("mining");
    _skill(SKILL_MINING, 3);
    _resource_export(RES_STONE);
    _mineral_export(MINERAL_TIN);
    _mineral_export(MINERAL_COPPER);
    _mineral_export(MINERAL_IRON);
    _mineral_export(MINERAL_COAL);

  _role(CITY_ROLE_LOGGING);
    _name("logging");
    _skill(SKILL_FORESTRY, 2);
    _resource_export(RES_WOOD);
}
