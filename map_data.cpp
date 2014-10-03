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
#define _crop(c) \
  Terrain_data[cur_id]->crops.push_back( (c) )
#define _mineral(m, a) \
  Terrain_data[cur_id]->minerals.push_back( Mineral_amount( (m), (a) ) )
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
    _farm(10);
    _crop(CROP_CABBAGE);
    _crop(CROP_AMANITAS);
    _mineral(MINERAL_TIN,       300);
    _mineral(MINERAL_COPPER,    300);
    _mineral(MINERAL_GOLD,      800);
    _mineral(MINERAL_COAL,     1000);
    _buildable(AREA_HOVELS);
    _buildable(AREA_HOUSES);
    _buildable(AREA_MANOR);
    _buildable(AREA_KEEP);
    _buildable(AREA_FARM);
    _buildable(AREA_BARRACKS);

  _ter(TER_ICY_HILL);
    _name("icy hill");
    _symbol('n', c_ltgray);
    _farm(0);
    _mineral(MINERAL_STONE, INFINITE_RESOURCE);
    _mineral(MINERAL_TIN,      2000);
    _mineral(MINERAL_COPPER,   5000);
    _mineral(MINERAL_IRON,     5000);
    _mineral(MINERAL_GOLD,     1500);
    _mineral(MINERAL_COAL,     4000);
    _buildable(AREA_HOVELS);
    _buildable(AREA_HOUSES);
// Burghers refuse to live on an icy hill!
    _buildable(AREA_KEEP);
    _buildable(AREA_QUARRY);
    _buildable(AREA_MINE);
    _buildable(AREA_BARRACKS);

  _ter(TER_ICY_MOUNTAIN);
    _name("icy mountain");
    _symbol('^', c_white);
    _farm(0);
    _mineral(MINERAL_STONE, INFINITE_RESOURCE);
    _mineral(MINERAL_TIN,      5000);
    _mineral(MINERAL_COPPER,  10000);
    _mineral(MINERAL_IRON,    10000);
    _mineral(MINERAL_GOLD,     2000);
    _mineral(MINERAL_COAL,     8000);
    _buildable(AREA_HOVELS);
// Merchants and burghers refuse to live on an icy mountain!
    _buildable(AREA_KEEP);
    _buildable(AREA_MINE);

  _ter(TER_GLACIER);
    _name("glacier");
    _symbol('#', c_ltcyan);
    _farm(0);
// Glaciers are totally useless!
    

  _ter(TER_FIELD);
    _name("Field");
    _symbol('.', c_green);
    _farm(100);
    _crop(CROP_WHEAT);
    _crop(CROP_CABBAGE);
    _crop(CROP_TOBACCO);
    _crop(CROP_COTTON);
    _crop(CROP_HAY);
    _buildable(AREA_HOVELS);
    _buildable(AREA_HOUSES);
    _buildable(AREA_MANOR);
    _buildable(AREA_KEEP);
    _buildable(AREA_FARM);
    _buildable(AREA_BARRACKS);

  _ter(TER_ROCKY);
    _name("Rocky");
    _symbol(',', c_brown);
    _farm(60);
    _crop(CROP_WHEAT);
    _crop(CROP_CABBAGE);
    _crop(CROP_TOBACCO);
    _crop(CROP_HEMP);
    _crop(CROP_HAY);
    _mineral(MINERAL_STONE,   10000);
    _mineral(MINERAL_TIN,       500);
    _mineral(MINERAL_COPPER,   1000);
    _mineral(MINERAL_IRON,     1000);
    _mineral(MINERAL_GOLD,      200);
    _mineral(MINERAL_COAL,      500);
    _buildable(AREA_HOVELS);
    _buildable(AREA_HOUSES);
    _buildable(AREA_MANOR);
    _buildable(AREA_KEEP);
    _buildable(AREA_FARM);
    _buildable(AREA_QUARRY);
    _buildable(AREA_MINE);
    _buildable(AREA_BARRACKS);

  _ter(TER_HILL);
    _name("Hill");
    _symbol('=', c_brown);
    _farm(50);
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
    _buildable(AREA_HOVELS);
    _buildable(AREA_HOUSES);
    _buildable(AREA_MANOR);
    _buildable(AREA_KEEP);
    _buildable(AREA_FARM);
    _buildable(AREA_QUARRY);
    _buildable(AREA_MINE);
    _buildable(AREA_BARRACKS);

  _ter(TER_MOUNTAIN);
    _name("Mountain");
    _symbol('^', c_ltgray);
    _farm(10);
    _crop(CROP_WHEAT);
    _crop(CROP_MELON);
    _mineral(MINERAL_STONE, INFINITE_RESOURCE);
    _mineral(MINERAL_TIN,      5000);
    _mineral(MINERAL_COPPER,  10000);
    _mineral(MINERAL_IRON,    10000);
    _mineral(MINERAL_GOLD,     2000);
    _mineral(MINERAL_COAL,     8000);
    _buildable(AREA_HOVELS);
    _buildable(AREA_HOUSES);
    _buildable(AREA_MANOR);
    _buildable(AREA_KEEP);
    _buildable(AREA_FARM);
    _buildable(AREA_MINE);

  _ter(TER_FOREST);
    _name("Forest");
    _symbol('7', c_ltgreen);
    _farm(20);
    _crop(CROP_CABBAGE);
    _crop(CROP_MELON);
    _crop(CROP_AMANITAS);
    _crop(CROP_DEATHCAP);
    _crop(CROP_CINNAMON);
    _mineral(MINERAL_COAL,     4000);
    _buildable(AREA_HOVELS);
// Houses and manors take up too much space to be built in a forest!
    _buildable(AREA_KEEP);
    _buildable(AREA_FARM);

  _ter(TER_RIVER);
    _name("River");
    _symbol('#', c_blue);
    _farm(0);
    _mineral(MINERAL_COPPER,    100);
    _mineral(MINERAL_GOLD,      100);
// Nothing is buildable on river! (maybe docks later on)

  _ter(TER_OCEAN);
    _name("Ocean");
    _symbol('#', c_ltblue);
    _farm(0);
// Nothing is buildable on ocean! (maybe docks later on)

  _ter(TER_DESERT);
    _name("Desert");
    _symbol('~', c_yellow);
    _farm(30);
    _crop(CROP_CACTUS);
    _crop(CROP_SPICEREED);
    _crop(CROP_SCORPICON);
    _mineral(MINERAL_COPPER,    800);
    _mineral(MINERAL_IRON,      500);
    _mineral(MINERAL_GOLD,      100);
    _buildable(AREA_HOVELS);
    _buildable(AREA_HOUSES);
    _buildable(AREA_MANOR);
    _buildable(AREA_KEEP);
    _buildable(AREA_FARM);
    _buildable(AREA_QUARRY);
    _buildable(AREA_BARRACKS);

  _ter(TER_SWAMP);
    _name("Swamp");
    _symbol('~', c_cyan);
    _farm(80);
    _crop(CROP_RICE);
    _crop(CROP_PEPPER);
    _crop(CROP_CINNAMON);
    _crop(CROP_AYAHUASCA);
    _crop(CROP_VIPERVINE);
    _mineral(MINERAL_IRON,     2000);
    _mineral(MINERAL_COAL,     5000);
    _buildable(AREA_HOVELS);
// Merchants and burghers refuse to live in the swamp!
    _buildable(AREA_KEEP);
    _buildable(AREA_FARM);
    _buildable(AREA_QUARRY);

  _ter(TER_ICE);
    _name("Ice");
    _symbol('~', c_ltcyan);
    _farm(0);
    _buildable(AREA_HOVELS);
    _buildable(AREA_HOUSES);
    _buildable(AREA_BARRACKS);
}
