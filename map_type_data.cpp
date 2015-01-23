#include "map.h"

Map_type_datum* Map_type_data[MAP_MAX];

#define _ter(n) \
  cur_id = (n)

#define _name(n) \
  Map_type_data[cur_id]->name = (n)

#define _water() \
  Map_type_data[cur_id]->is_water = true

#define _river() \
  Map_type_data[cur_id]->is_river = true

#define _road_cost(n) \
  Map_type_data[cur_id]->road_cost = (n)

#define _travel_cost(n) \
  Map_type_data[cur_id]->travel_cost = (n)

#define _symbol(c, f) \
  Map_type_data[cur_id]->symbol = glyph( (c), (f), c_black)

#define _role(r) \
  Map_type_data[cur_id]->city_roles.push_back( (r) )

void init_map_type_data()
{
  int cur_id = 0;
  for (int i = 0; i < MAP_MAX; i++) {
    Map_type_data[i] = new Map_type_datum;
  }

  _ter(MAP_NULL);
    _name("Out of bounds");
    _road_cost(-1);   // Roads are forbidden
    _travel_cost(-1); // Can't travel here
    _symbol('x', c_red);

  _ter(MAP_TUNDRA);
    _name("tundra");
    _road_cost(50);
    _travel_cost(50);
    _symbol('*', c_white);
    _role(CITY_ROLE_HUNTING);

  _ter(MAP_DESERT);
    _name("desert");
    _road_cost(35);
    _travel_cost(40);
    _symbol('x', c_yellow);
    _role(CITY_ROLE_HUNTING);

  _ter(MAP_WASTELAND);
    _name("wasteland");
    _road_cost(13);
    _travel_cost(40);
    _symbol(',', c_brown);
    _role(CITY_ROLE_FARMING);
    _role(CITY_ROLE_MINING);
    _role(CITY_ROLE_HUNTING);

  _ter(MAP_PLAINS);
    _name("plains");
    _road_cost(10);
    _travel_cost(25);
    _symbol('.', c_green);
    _role(CITY_ROLE_FARMING);
    _role(CITY_ROLE_HUNTING);
    _role(CITY_ROLE_LIVESTOCK);

  _ter(MAP_FOREST);
    _name("forest");
    _road_cost(25);
    _travel_cost(75);
    _symbol('7', c_ltgreen);
    _role(CITY_ROLE_LOGGING);
    _role(CITY_ROLE_HUNTING);

  _ter(MAP_SWAMP);
    _name("swamp");
    _road_cost(40);
    _travel_cost(125);
    _water();
    _symbol('s', c_cyan);
    _role(CITY_ROLE_FARMING);

  _ter(MAP_JUNGLE);
    _name("jungle");
    _road_cost(75);
    _travel_cost(200);
    _symbol('J', c_ltcyan);
    _role(CITY_ROLE_LOGGING);
    _role(CITY_ROLE_HUNTING);

  _ter(MAP_FOOTHILLS);
    _name("foothills");
    _road_cost(20);
    _travel_cost(50);
    _symbol('n', c_brown);
    _role(CITY_ROLE_FARMING);
    _role(CITY_ROLE_MINING);
    _role(CITY_ROLE_HUNTING);
    _role(CITY_ROLE_LIVESTOCK);

  _ter(MAP_ICY_FOOTHILLS);
    _name("icy foothills");
    _road_cost(35);
    _travel_cost(100);
    _symbol('n', c_ltcyan);
    _role(CITY_ROLE_MINING);
    _role(CITY_ROLE_HUNTING);

  _ter(MAP_MOUNTAINOUS);
    _name("mountain range");
    _road_cost(100);
    _travel_cost(250);
    _symbol('M', c_ltgray);
    _role(CITY_ROLE_MINING);

  _ter(MAP_ICY_MOUNTAIN);
    _name("icy mountain");
    _road_cost(-1);
    _travel_cost(400);
    _symbol('M', c_white);
    _role(CITY_ROLE_MINING);

  _ter(MAP_BASIN);
    _name("river basin");
    _road_cost(25);
    _travel_cost(20);
    _river();
    _water();
    _symbol('S', c_blue);
    _role(CITY_ROLE_FARMING);
    _role(CITY_ROLE_HUNTING);
    _role(CITY_ROLE_LIVESTOCK);

  _ter(MAP_CANYON);
    _name("canyon");
    _road_cost(30);
    _travel_cost(40);
    _river();
    _water();
    _symbol('S', c_ltblue);
    _role(CITY_ROLE_FARMING);
    _role(CITY_ROLE_MINING);
    _role(CITY_ROLE_HUNTING);

  _ter(MAP_GLACIER);
    _name("glacier");
    _road_cost(40);
    _travel_cost(75);
    _river();
    _water();
    _symbol('S', c_ltcyan);

  _ter(MAP_COASTAL);
    _name("coastal");
    _road_cost(9);
    _travel_cost(25);
    _symbol('.', c_yellow);
    _role(CITY_ROLE_FARMING);

  _ter(MAP_OCEAN);
    _name("ocean");
    _road_cost(-1);
    _travel_cost(-1);
    _water();
    _symbol('=', c_blue);

  _ter(MAP_ICECAP);
    _name("icecap");
    _road_cost(100);
    _travel_cost(100);
    _symbol('=', c_ltcyan);
}
