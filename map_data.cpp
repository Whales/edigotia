#include "map.h"

Terrain_datum* Terrain_data[TER_MAX];

#define _ter(n) \
  cur_id = (n)

#define _name(n) \
  Terrain_data[cur_id]->name = (n)

#define _symbol(c, f) \
  Terrain_data[cur_id]->symbol = glyph((c), (f), (c_black))

#define _farm(n) \
  Terrain_data[cur_id]->farm_percent = (n)

#define _hunting_difficulty(n) \
  Terrain_data[cur_id]->hunting_difficulty = (n)

#define _wood(a, b) \
  Terrain_data[cur_id]->wood_min = (a); \
  Terrain_data[cur_id]->wood_max = (b)

#define _wood_cleared(t) \
  Terrain_data[cur_id]->wood_cleared_type = (t)

#define _crop(c) \
  Terrain_data[cur_id]->crops.push_back( (c) )

#define _mineral(m, n) \
  Terrain_data[cur_id]->minerals.push_back( Mineral_amount( (m), (n) ) )

#define _animal(a, n) \
  Terrain_data[cur_id]->animals.push_back( Animal_amount( (a), (n) ) )

#define _buildable(a) \
  Terrain_data[cur_id]->buildable_areas.push_back( (a) )

void init_terrain_data()
{
  for (int i = 0; i < TER_MAX; i++) {
    Terrain_data[i] = new Terrain_datum;
  }
  int cur_id = 0;

  _ter(TER_NULL);
    _name("Out of bounds");
    _symbol('x', c_red);

  _ter(TER_TUNDRA);
    _name("tundra");
    _symbol('*', c_white);
    _farm(20);
    _hunting_difficulty(12000);
    _wood(0, 400);
    _crop(CROP_CABBAGE);
    _crop(CROP_AMANITAS);
    _mineral(MINERAL_TIN,       300);
    _mineral(MINERAL_COPPER,    300);
    _mineral(MINERAL_GOLD,      800);
    _mineral(MINERAL_COAL,     1000);
    _animal(ANIMAL_HARE,       2000);
    _animal(ANIMAL_FOX,         600);
    _animal(ANIMAL_BEAR,        100);
    _animal(ANIMAL_WOLF,        250);
    _animal(ANIMAL_DIRE_WOLF,    50);
    _animal(ANIMAL_AKHLUT,       40);
    _buildable(AREA_HOVELS);
    _buildable(AREA_HOUSES);
    _buildable(AREA_MANOR);
    _buildable(AREA_KEEP);
    _buildable(AREA_FARM);
    _buildable(AREA_HUNTING_CAMP);
    _buildable(AREA_PASTURE);
    _buildable(AREA_BARRACKS);

  _ter(TER_ICY_HILL);
    _name("icy hill");
    _symbol('=', c_ltgray);
    _farm(12);
    _hunting_difficulty(18000);
    _wood(0, 350);
    _mineral(MINERAL_STONE, INFINITE_RESOURCE);
    _mineral(MINERAL_TIN,      2000);
    _mineral(MINERAL_COPPER,   5000);
    _mineral(MINERAL_IRON,     5000);
    _mineral(MINERAL_GOLD,     1500);
    _mineral(MINERAL_COAL,     4000);
    _animal(ANIMAL_HARE,       1600);
    _animal(ANIMAL_FOX,         480);
    _animal(ANIMAL_BEAR,         70);
    _animal(ANIMAL_WOLF,        180);
    _animal(ANIMAL_DIRE_WOLF,    35);
    _animal(ANIMAL_AKHLUT,       15);
    _buildable(AREA_HOVELS);
    _buildable(AREA_HOUSES);
// Burghers refuse to live on an icy hill!
    _buildable(AREA_KEEP);
    _buildable(AREA_HUNTING_CAMP);
    _buildable(AREA_PASTURE);
    _buildable(AREA_MINE);
    _buildable(AREA_BARRACKS);

  _ter(TER_ICY_MOUNTAIN);
    _name("icy mountain");
    _symbol('^', c_white);
    _farm(0);
    _hunting_difficulty(15000);
    _wood(0, 100);
    _mineral(MINERAL_STONE, INFINITE_RESOURCE);
    _mineral(MINERAL_TIN,      5000);
    _mineral(MINERAL_COPPER,  10000);
    _mineral(MINERAL_IRON,    10000);
    _mineral(MINERAL_GOLD,     2000);
    _mineral(MINERAL_COAL,     8000);
    _animal(ANIMAL_FOX,         120);
    _animal(ANIMAL_BEAR,         50);
    _animal(ANIMAL_DIRE_WOLF,     5);
    _buildable(AREA_HOVELS);
// Merchants and burghers refuse to live on an icy mountain!
    _buildable(AREA_KEEP);
    _buildable(AREA_HUNTING_CAMP);
    _buildable(AREA_MINE);

  _ter(TER_GLACIER);
    _name("glacier");
    _symbol('#', c_ltcyan);
    _farm(0);
    _hunting_difficulty(15000);
    _animal(ANIMAL_AKHLUT,      150);
    _buildable(AREA_HUNTING_CAMP);
// Glaciers are totally useless!  Except for Akhluts, which are very dangerous.
    

  _ter(TER_FIELD);
    _name("Field");
    _symbol('_', c_green);
    _farm(100);
    _hunting_difficulty(5000);
    _wood(100, 1000);
    _crop(CROP_WHEAT);
    _crop(CROP_CABBAGE);
    _crop(CROP_TOBACCO);
    _crop(CROP_COTTON);
    _animal(ANIMAL_HARE,       5000);
    _animal(ANIMAL_FOX,        1000);
    _animal(ANIMAL_BOAR,        200);
    _animal(ANIMAL_DEER,        400);
    _animal(ANIMAL_WOLF,        300);
    _animal(ANIMAL_LION,        300);
    _animal(ANIMAL_ELEPHANT,    200);
    _animal(ANIMAL_JELLY,       100);
    _animal(ANIMAL_DIRE_WOLF,    12);
    _animal(ANIMAL_UNICORN,       2);
    _buildable(AREA_PARK);
    _buildable(AREA_PLAZA);
    _buildable(AREA_MARKETPLACE);
    _buildable(AREA_HOVELS);
    _buildable(AREA_HOUSES);
    _buildable(AREA_MANOR);
    _buildable(AREA_KEEP);
    _buildable(AREA_FARM);
    _buildable(AREA_HUNTING_CAMP);
    _buildable(AREA_PASTURE);
    _buildable(AREA_SAWMILL);
    _buildable(AREA_BARRACKS);

  _ter(TER_ROCKY);
    _name("Rocky");
    _symbol(',', c_brown);
    _farm(85);
    _hunting_difficulty(10000);
    _wood(0, 200);
    _crop(CROP_WHEAT);
    _crop(CROP_CABBAGE);
    _crop(CROP_TOBACCO);
    _crop(CROP_HEMP);
    _mineral(MINERAL_STONE,   10000);
    _mineral(MINERAL_TIN,       500);
    _mineral(MINERAL_COPPER,   1000);
    _mineral(MINERAL_IRON,     1000);
    _mineral(MINERAL_GOLD,      200);
    _mineral(MINERAL_COAL,      500);
    _animal(ANIMAL_HARE,       4500);
    _animal(ANIMAL_FOX,        1000);
    _animal(ANIMAL_BOAR,       1200);
    _animal(ANIMAL_WOLF,        800);
    _animal(ANIMAL_LION,        200);
    _animal(ANIMAL_JELLY,       100);
    _animal(ANIMAL_DIRE_WOLF,    60);
    _animal(ANIMAL_TIGER,        40);
    _animal(ANIMAL_BASILISK,     12);
    _buildable(AREA_PARK);
    _buildable(AREA_PLAZA);
    _buildable(AREA_MARKETPLACE);
    _buildable(AREA_HOVELS);
    _buildable(AREA_HOUSES);
    _buildable(AREA_MANOR);
    _buildable(AREA_KEEP);
    _buildable(AREA_FARM);
    _buildable(AREA_HUNTING_CAMP);
    _buildable(AREA_PASTURE);
    _buildable(AREA_MINE);
    _buildable(AREA_SAWMILL);
    _buildable(AREA_BARRACKS);

  _ter(TER_HILL);
    _name("Hill");
    _symbol('=', c_brown);
    _farm(75);
    _hunting_difficulty(8000);
    _wood(50, 300);
    _crop(CROP_WHEAT);
    _crop(CROP_CABBAGE);
    _crop(CROP_GRAPES);
    _crop(CROP_MELON);
    _crop(CROP_TOBACCO);
    _crop(CROP_HEMP);
    _mineral(MINERAL_STONE, INFINITE_RESOURCE);
    _mineral(MINERAL_TIN,      2000);
    _mineral(MINERAL_COPPER,   5000);
    _mineral(MINERAL_IRON,     5000);
    _mineral(MINERAL_GOLD,     1500);
    _mineral(MINERAL_COAL,     4000);
    _animal(ANIMAL_HARE,       4600);
    _animal(ANIMAL_FOX,        1000);
    _animal(ANIMAL_BOAR,       1000);
    _animal(ANIMAL_DEER,        800);
    _animal(ANIMAL_BEAR,        250);
    _animal(ANIMAL_WOLF,        400);
    _animal(ANIMAL_JELLY,       100);
    _animal(ANIMAL_DIRE_WOLF,    60);
    _animal(ANIMAL_TIGER,        40);
    _animal(ANIMAL_MANTICORE,    15);
    _animal(ANIMAL_BASILISK,     18);
    _buildable(AREA_PARK);
    _buildable(AREA_MARKETPLACE);
    _buildable(AREA_HOVELS);
    _buildable(AREA_HOUSES);
    _buildable(AREA_MANOR);
    _buildable(AREA_KEEP);
    _buildable(AREA_FARM);
    _buildable(AREA_HUNTING_CAMP);
    _buildable(AREA_PASTURE);
    _buildable(AREA_MINE);
    _buildable(AREA_SAWMILL);
    _buildable(AREA_BARRACKS);

  _ter(TER_MOUNTAIN);
    _name("Mountain");
    _symbol('^', c_ltgray);
    _farm(50);
    _hunting_difficulty(12000);
    _wood(0, 200);
    _crop(CROP_WHEAT);
    _crop(CROP_MELON);
    _mineral(MINERAL_STONE, INFINITE_RESOURCE);
    _mineral(MINERAL_TIN,      5000);
    _mineral(MINERAL_COPPER,  10000);
    _mineral(MINERAL_IRON,    10000);
    _mineral(MINERAL_GOLD,     2000);
    _mineral(MINERAL_COAL,     8000);
    _animal(ANIMAL_HARE,        800);
    _animal(ANIMAL_BEAR,        200);
    _animal(ANIMAL_JELLY,       100);
    _animal(ANIMAL_DIRE_WOLF,   100);
    _animal(ANIMAL_MANTICORE,    80);
    _animal(ANIMAL_BASILISK,     30);
    _animal(ANIMAL_WYVERN,       12);
    _buildable(AREA_HOVELS);
    _buildable(AREA_HOUSES);
    _buildable(AREA_MANOR);
    _buildable(AREA_KEEP);
    _buildable(AREA_FARM);
    _buildable(AREA_HUNTING_CAMP);
    _buildable(AREA_MINE);

  _ter(TER_FOREST);
    _name("Forest");
    _symbol(';', c_ltgreen);
    _farm(60);
    _hunting_difficulty(25000);
    _wood(20000, 80000);
    _wood_cleared(TER_BARREN);
    _crop(CROP_CABBAGE);
    _crop(CROP_MELON);
    _crop(CROP_AMANITAS);
    _crop(CROP_DEATHCAP);
    _crop(CROP_CINNAMON);
    _mineral(MINERAL_STONE,    1500);
    _mineral(MINERAL_COAL,     4000);
    _animal(ANIMAL_HARE,       7500);
    _animal(ANIMAL_FOX,        2400);
    _animal(ANIMAL_DEER,       4000);
    _animal(ANIMAL_BEAR,       1000);
    _animal(ANIMAL_WOLF,        500);
    _animal(ANIMAL_JELLY,       200);
    _animal(ANIMAL_MAN_EATING_PLANT,  20);
    _animal(ANIMAL_DIRE_WOLF,    50);
    _animal(ANIMAL_UNICORN,      80);
    _animal(ANIMAL_BASILISK,     10);
    _buildable(AREA_PARK);
    _buildable(AREA_HOVELS);
// Houses and manors take up too much space to be built in a forest!
    _buildable(AREA_KEEP);
    _buildable(AREA_FARM);
    _buildable(AREA_HUNTING_CAMP);
    _buildable(AREA_SAWMILL);

// TER_BARREN should only appear after a forest has been totally cleared.
  _ter(TER_BARREN);
    _name("Barren");
    _symbol(':', c_brown);
    _farm(90);
    _hunting_difficulty(12000);
    _crop(CROP_CABBAGE);
    _crop(CROP_MELON);
    _crop(CROP_AMANITAS);
    _crop(CROP_DEATHCAP);
    _crop(CROP_CINNAMON);
    _mineral(MINERAL_STONE,    1500);
    _mineral(MINERAL_COAL,     4000);
    _animal(ANIMAL_HARE,       2500);
    _animal(ANIMAL_FOX,         800);
    _animal(ANIMAL_DEER,       1200);
    _animal(ANIMAL_BEAR,        300);
    _animal(ANIMAL_WOLF,        120);
    _animal(ANIMAL_JELLY,       100);
    _animal(ANIMAL_DIRE_WOLF,    20);
    _animal(ANIMAL_UNICORN,      10);
    _animal(ANIMAL_BASILISK,     12);
    _buildable(AREA_PARK);
    _buildable(AREA_PLAZA);
    _buildable(AREA_MARKETPLACE);
    _buildable(AREA_HOVELS);
    _buildable(AREA_HOUSES);
    _buildable(AREA_MANOR);
    _buildable(AREA_KEEP);
    _buildable(AREA_FARM);
    _buildable(AREA_HUNTING_CAMP);
    _buildable(AREA_PASTURE);
    _buildable(AREA_MINE);
    _buildable(AREA_BARRACKS);

  _ter(TER_RIVER);
    _name("River");
    _symbol('#', c_blue);
    _farm(0);
    _mineral(MINERAL_COPPER,    100);
    _mineral(MINERAL_GOLD,      100);
// Nothing is buildable on river! (maybe docks later on)

  _ter(TER_LAKE);
    _name("Lake");
    _symbol('#', c_cyan);
    _farm(0);
    _mineral(MINERAL_GOLD, 250);
// Nothing is buildable on lake!

  _ter(TER_OCEAN);
    _name("Ocean");
    _symbol('#', c_ltblue);
    _farm(0);
// Nothing is buildable on ocean! (maybe docks later on)

  _ter(TER_DESERT);
    _name("Desert");
    _symbol('~', c_yellow);
    _farm(50);
    _hunting_difficulty(5000);
    _crop(CROP_CACTUS);
    _crop(CROP_SPICEREED);
    _crop(CROP_SCORPICON);
    _mineral(MINERAL_COPPER,    800);
    _mineral(MINERAL_IRON,      500);
    _mineral(MINERAL_GOLD,      100);
    _animal(ANIMAL_HARE,       1000);
    _animal(ANIMAL_FOX,         300);
    _animal(ANIMAL_BOAR,        500);
    _animal(ANIMAL_LION,        300);
    _animal(ANIMAL_ELEPHANT,    150);
    _animal(ANIMAL_JELLY,        50);
    _animal(ANIMAL_MANTICORE,   120);
    _animal(ANIMAL_BASILISK,     50);
    _buildable(AREA_PLAZA);
    _buildable(AREA_MARKETPLACE);
    _buildable(AREA_HOVELS);
    _buildable(AREA_HOUSES);
    _buildable(AREA_MANOR);
    _buildable(AREA_KEEP);
    _buildable(AREA_FARM);
    _buildable(AREA_HUNTING_CAMP);
    _buildable(AREA_PASTURE);
    _buildable(AREA_BARRACKS);

  _ter(TER_SWAMP);
    _name("Swamp");
    _symbol('~', c_cyan);
    _farm(90);
    _hunting_difficulty(30000);
    _wood(5000, 40000);
    _crop(CROP_RICE);
    _crop(CROP_PEPPER);
    _crop(CROP_CINNAMON);
    _crop(CROP_AYAHUASCA);
    _crop(CROP_VIPERVINE);
    _mineral(MINERAL_IRON,     2000);
    _mineral(MINERAL_COAL,     5000);
    _animal(ANIMAL_HARE,       2500);
    _animal(ANIMAL_FOX,        1000);
    _animal(ANIMAL_DEER,       1000);
    _animal(ANIMAL_BEAR,        600);
    _animal(ANIMAL_WOLF,        200);
    _animal(ANIMAL_JELLY,       300);
    _animal(ANIMAL_MAN_EATING_PLANT, 400);
    _animal(ANIMAL_DIRE_WOLF,    30);
    _animal(ANIMAL_UNICORN,      40);
    _buildable(AREA_HOVELS);
// Merchants and burghers refuse to live in the swamp!
    _buildable(AREA_KEEP);
    _buildable(AREA_FARM);
    _buildable(AREA_HUNTING_CAMP);
    _buildable(AREA_MINE);
    _buildable(AREA_SAWMILL);

  _ter(TER_JUNGLE);
    _name("Jungle");
    _symbol('J', c_ltcyan);
    _farm(50);
    _hunting_difficulty(30000);
    _wood(30000, 100000);
    _wood_cleared(TER_FIELD);
    _crop(CROP_RICE);
    _crop(CROP_PEPPER);
    _crop(CROP_CINNAMON);
    _crop(CROP_AYAHUASCA);
    _crop(CROP_DEATHCAP);
    _crop(CROP_VIPERVINE);
    _crop(CROP_SCORPICON);
    _mineral(MINERAL_STONE,    1800);
    _mineral(MINERAL_COAL,     6000);
    _animal(ANIMAL_BOAR,       1400);
    _animal(ANIMAL_TIGER,      1000);
    _animal(ANIMAL_ELEPHANT,     50);
    _animal(ANIMAL_JELLY,       100);
    _animal(ANIMAL_MAN_EATING_PLANT, 400);
    _animal(ANIMAL_MANTICORE,   200);
    _buildable(AREA_HUNTING_CAMP);
    _buildable(AREA_SAWMILL);

  _ter(TER_ICE);
    _name("Ice");
    _symbol('~', c_ltcyan);
    _farm(0);
    _hunting_difficulty(1000);
    _animal(ANIMAL_AKHLUT,       50);
    _buildable(AREA_PLAZA);
    _buildable(AREA_MARKETPLACE);
    _buildable(AREA_HOVELS);
    _buildable(AREA_HOUSES);
    _buildable(AREA_HUNTING_CAMP);
    _buildable(AREA_BARRACKS);
}
