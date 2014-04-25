#include "building.h"

Building_datum* Building_data[BUILD_MAX];

#define _build(x) cur_id = (x)
#define _cost(t, n) Building_data[cur_id]->build_costs.push_back( \
  Resource_cost( (t), (n) ) )
#define _upkeep(n) Building_data[cur_id]->upkeep = (n)
#define _housing(t, n) Building_data[cur_id]->housing = Citizen_amount((t), (n))
#define _jobs(n) Building_data[cur_id]->jobs = (n)
#define _flat(t, n) Building_data[cur_id]->flat_production.push_back( \
  Resource_cost( (t), (n) ) )
#define _buildable(t) Building_data[cur_id]->buildable.push_back( (t) )

void init_building_data()
{
  for (int i = 0; i < BUILD_MAX; i++) {
    Building_data[i] = new Building_datum;
    Building_data[i]->uid = i;
  }

  int cur_id;

  _build(BUILD_HOVEL);
    _cost(RES_GOLD, 100);
    _cost(RES_WOOD, 10);
    _upkeep(5);
    _housing(CIT_PEASANT, 100);

  _build(BUILD_HOUSE);
    _cost(RES_GOLD, 400);
    _cost(RES_WOOD, 20);
    _upkeep(8);
    _housing(CIT_MERCHANT, 100);

  _build(BUILD_MANOR);
    _cost(RES_GOLD, 800);
    _cost(RES_STONE, 20);
    _upkeep(12);
    _housing(CIT_BURGHER, 50);

  _build(BUILD_PALACE);
    _cost(RES_GOLD, 1500);
    _cost(RES_STONE, 50);
    _upkeep(20);
    _housing(CIT_NOBLE, 20);
}
