#include "race.h"

Race_datum* Race_data[RACE_MAX];


#define _race(n) \
  cur_id = (n)

#define _name(s) \
  Race_data[cur_id]->name = (s)

#define _plural_name(s) \
  Race_data[cur_id]->plural_name = (s)

#define _adjective(s) \
  Race_data[cur_id]->adjective = (s)

#define _color(c) \
  Race_data[cur_id]->color = (c)

#define _kingdom_color(c) \
  Race_data[cur_id]->kingdom_colors.push_back( (c) )

#define _map_type(m, i) \
  Race_data[cur_id]->map_type_value[(m)] = (i)

#define _food_consumption(n) \
  Race_data[cur_id]->food_consumption = (n)

#define _starting_population(t, n) \
  Race_data[cur_id]->starting_population[ (t) ] = (n)

#define _birth_rate(t, n) \
  Race_data[cur_id]->birth_rate[ (t) ] = (n)

#define _citizen_ratio(t, n) \
  Race_data[cur_id]->citizen_ratio[ (t) ] = (n)

#define _morale_requirement(t, n) \
  Race_data[cur_id]->morale_requirement[ (t) ] = (n)

#define _low_tax_rate(t, n) \
  Race_data[cur_id]->low_tax_rate[ (t) ] = (n)

#define _high_tax_rate(t, n) \
  Race_data[cur_id]->high_tax_rate[ (t) ] = (n)

#define _skill(t, n) \
  Race_data[cur_id]->skill_level[ (t) ] = (n)

void init_races()
{
  for (int i = 0; i < RACE_MAX; i++) {
    Race_data[i] = new Race_datum();
    Race_data[i]->uid = i;
  }

  int cur_id = 0;

  _race(RACE_HUMAN);
    _name("human");
    _plural_name("humans");
    _adjective("human");

    _color(c_white);
    _kingdom_color(c_ltgray);
    _kingdom_color(c_cyan);
    _kingdom_color(c_white);
    _kingdom_color(c_ltblue);
    _kingdom_color(c_ltcyan);
    _kingdom_color(c_yellow);

    _map_type(MAP_PLAINS,      100);
    _map_type(MAP_BASIN,        80);
    _map_type(MAP_CANYON,       50);
    _map_type(MAP_FOREST,       40);
    _map_type(MAP_FOOTHILLS,    10);

    _map_type(MAP_DESERT,      -40);
    _map_type(MAP_SWAMP,       -20);
    _map_type(MAP_MOUNTAINOUS,  -5);

    _food_consumption(100);

    _starting_population(CIT_PEASANT,  100);
    _birth_rate         (CIT_PEASANT,  100);
    _birth_rate         (CIT_MERCHANT,  90);
    _birth_rate         (CIT_BURGHER,   80);
    _citizen_ratio      (CIT_MERCHANT,  10);
    _citizen_ratio      (CIT_BURGHER,   10);
    _morale_requirement (CIT_MERCHANT,  50);
    _morale_requirement (CIT_BURGHER,   70);
    _low_tax_rate       (CIT_PEASANT,   20);
    _low_tax_rate       (CIT_MERCHANT,  15);
    _low_tax_rate       (CIT_BURGHER,   10);
    _high_tax_rate      (CIT_PEASANT,   80);
    _high_tax_rate      (CIT_MERCHANT,  65);
    _high_tax_rate      (CIT_BURGHER,   50);

    _skill(SKILL_FARMING,   4);
    _skill(SKILL_MINING,    3);
    _skill(SKILL_FORESTRY,  3);

  _race(RACE_ELF);
    _name("elf");
    _plural_name("elves");
    _adjective("elvish");

    _color(c_ltgreen);
    _kingdom_color(c_green);
    _kingdom_color(c_cyan);
    _kingdom_color(c_brown);
    _kingdom_color(c_ltgreen);
    _kingdom_color(c_ltcyan);

    _map_type(MAP_FOREST,      100);
    _map_type(MAP_BASIN,        30);
    _map_type(MAP_SWAMP,        15);
    _map_type(MAP_PLAINS,       10);

    _map_type(MAP_DESERT,      -40);
    _map_type(MAP_MOUNTAINOUS, -30);

    _food_consumption(95);

    _starting_population(CIT_PEASANT,  100);
    _birth_rate         (CIT_PEASANT,  120);
    _birth_rate         (CIT_MERCHANT, 110);
    _birth_rate         (CIT_BURGHER,  100);
    _citizen_ratio      (CIT_MERCHANT,   8);
    _citizen_ratio      (CIT_BURGHER,   12);
    _morale_requirement (CIT_MERCHANT,  65);
    _morale_requirement (CIT_BURGHER,   75);
    _low_tax_rate       (CIT_PEASANT,   15);
    _low_tax_rate       (CIT_MERCHANT,  10);
    _low_tax_rate       (CIT_BURGHER,    5);
    _high_tax_rate      (CIT_PEASANT,   75);
    _high_tax_rate      (CIT_MERCHANT,  75);
    _high_tax_rate      (CIT_BURGHER,   65);

    _skill(SKILL_FARMING,   4);
    _skill(SKILL_MINING,    2);
    _skill(SKILL_FORESTRY,  5);

  _race(RACE_DWARF);
    _name("dwarf");
    _plural_name("dwarves");
    _adjective("dwarvish");

    _color(c_brown);
    _kingdom_color(c_ltgray);
    _kingdom_color(c_red);
    _kingdom_color(c_brown);
    _kingdom_color(c_ltred);
    _kingdom_color(c_yellow);

    _map_type(MAP_CANYON,      100);
    _map_type(MAP_MOUNTAINOUS,  90);
    _map_type(MAP_FOOTHILLS,    70);
    _map_type(MAP_WASTELAND,    10);

    _map_type(MAP_DESERT,      -40);
    _map_type(MAP_PLAINS,      -20);

    _food_consumption(105);

    _starting_population(CIT_PEASANT,  100);
    _birth_rate         (CIT_PEASANT,  120);
    _birth_rate         (CIT_MERCHANT, 110);
    _birth_rate         (CIT_BURGHER,  100);
    _citizen_ratio      (CIT_MERCHANT,  10);
    _citizen_ratio      (CIT_BURGHER,   10);
    _morale_requirement (CIT_MERCHANT,  40);
    _morale_requirement (CIT_BURGHER,   60);
    _low_tax_rate       (CIT_PEASANT,   25);
    _low_tax_rate       (CIT_MERCHANT,  20);
    _low_tax_rate       (CIT_BURGHER,   15);
    _high_tax_rate      (CIT_PEASANT,   85);
    _high_tax_rate      (CIT_MERCHANT,  75);
    _high_tax_rate      (CIT_BURGHER,   65);

    _skill(SKILL_FARMING,   2);
    _skill(SKILL_MINING,    5);
    _skill(SKILL_FORESTRY,  2);


  _race(RACE_ORC);
    _name("orc");
    _plural_name("orcs");
    _adjective("orcish");

    _color(c_ltred);
    _kingdom_color(c_red);
    _kingdom_color(c_magenta);
    _kingdom_color(c_brown);
    _kingdom_color(c_ltred);
    _kingdom_color(c_yellow);

    _map_type(MAP_WASTELAND,  100);
    _map_type(MAP_FOOTHILLS,   80);
    _map_type(MAP_BASIN,       50);
    _map_type(MAP_CANYON,      45);
    _map_type(MAP_PLAINS,      40);
    _map_type(MAP_MOUNTAINOUS, 10);

    _map_type(MAP_DESERT,     -10);

    _food_consumption(105);

    _starting_population(CIT_PEASANT,  100);
    _birth_rate         (CIT_PEASANT,  100);
    _birth_rate         (CIT_MERCHANT,  95);
    _birth_rate         (CIT_BURGHER,   90);
    _citizen_ratio      (CIT_MERCHANT,  15);
    _citizen_ratio      (CIT_BURGHER,   10);
    _morale_requirement (CIT_MERCHANT,  30);
    _morale_requirement (CIT_BURGHER,   50);
    _low_tax_rate       (CIT_PEASANT,   30);
    _low_tax_rate       (CIT_MERCHANT,  25);
    _low_tax_rate       (CIT_BURGHER,   20);
    _high_tax_rate      (CIT_PEASANT,  100);
    _high_tax_rate      (CIT_MERCHANT,  85);
    _high_tax_rate      (CIT_BURGHER,   70);

    _skill(SKILL_FARMING,   1);
    _skill(SKILL_MINING,    4);
    _skill(SKILL_FORESTRY,  3);

}
