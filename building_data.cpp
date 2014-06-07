#include "building.h"

Building_datum* Building_data[BUILD_MAX];

#define _build(x) \
  cur_id = (x)
#define _name(n) \
  Building_data[cur_id]->name = (n)
#define _cost(t, n) \
  Building_data[cur_id]->build_costs.push_back( Resource_amount( (t), (n) ) )
#define _upkeep(n) \
  Building_data[cur_id]->upkeep = (n)
#define _maintenance(t, n) \
  Building_data[cur_id]->maintenance_cost.push_back( Resource_amount( (t),(n) ))
#define _housing(t, n) \
  Building_data[cur_id]->housing = Citizen_amount((t), (n))
#define _jobs(n) \
  Building_data[cur_id]->jobs = (n)
#define _flat(t, n) \
  Building_data[cur_id]->flat_production.push_back( Resource_amount( (t), (n) ))
#define _buildable(t) \
  Building_data[cur_id]->buildable.push_back( (t) )

void init_building_data()
{
  for (int i = 0; i < BUILD_MAX; i++) {
    Building_data[i] = new Building_datum;
    Building_data[i]->uid = i;
  }

  int cur_id;

  _build(BUILD_HOVEL);
    _name("hovel");
    _cost(RES_GOLD, 100);
    _cost(RES_WOOD, 10);
    _upkeep(5);
    _housing(CIT_PEASANT, 100);

  _build(BUILD_HOUSE);
    _name("house");
    _cost(RES_GOLD, 400);
    _cost(RES_WOOD, 20);
    _upkeep(8);
    _housing(CIT_MERCHANT, 100);

  _build(BUILD_MANOR);
    _name("manor");
    _cost(RES_GOLD, 800);
    _cost(RES_STONE, 20);
    _upkeep(12);
    _housing(CIT_BURGHER, 50);

  _build(BUILD_KEEP);
    _name("keep");
    _cost(RES_GOLD, 1500);
    _cost(RES_STONE, 50);
    _upkeep(20);
    _housing(CIT_NOBLE, 20);
    _housing(CIT_BURGHER, 5);
    _housing(CIT_MERCHANT, 50);
    _housing(CIT_PEASANT, 100);

  _build(BUILD_FARM);
    _name("farm");
    _cost(RES_GOLD, 100);
    _upkeep(3);
    _jobs(10);
    _flat(RES_FARMING, 10);

  _build(BUILD_QUARRY);
    _name("quarry");
    _cost(RES_GOLD, 500);
    _upkeep(2);
    _jobs(10);
    _flat(RES_STONE, 5);

  _build(BUILD_MINE);
    _name("mine");
    _cost(RES_GOLD, 500);
    _cost(RES_WOOD, 50);
    _upkeep(3);
    _jobs(10);
    _flat(RES_MINING, 10);

  _build(BUILD_CAMP);
    _name("hunting camp");
    _cost(RES_GOLD, 50);
    _jobs(10);
    _flat(RES_HUNTING, 10);

  _build(BUILD_SAWMILL);
    _name("sawmill");
    _cost(RES_GOLD, 100);
    _jobs(10);
    _flat(RES_LOGGING, 10);

  _build(BUILD_BARRACKS);
    _name("barracks");
    _cost(RES_GOLD, 500);
    _cost(RES_WOOD, 10);

  
}
