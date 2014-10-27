#include "building.h"
#include <sstream>

Building_datum* Building_data[BUILD_MAX];

#define _build(x) \
  cur_id = (x)

#define _name(n) \
  Building_data[cur_id]->name = (n)

#define _plural() \
  Building_data[cur_id]->plural = true

#define _description(n) \
  Building_data[cur_id]->description = (n)

#define _category(c) \
  Building_data[cur_id]->category = (c)

#define _cost(t, n) \
  Building_data[cur_id]->build_costs.push_back( Resource_amount( (t), (n) ) )

#define _destroy_cost(n) \
  Building_data[cur_id]->destroy_cost = (n)

#define _build_time(t) \
  Building_data[cur_id]->build_time = (t)

#define _upkeep(n) \
  Building_data[cur_id]->upkeep = (n)

#define _maintenance(t, n) \
  Building_data[cur_id]->maintenance_cost.push_back( Resource_amount( (t),(n) ))

#define _housing(t, n) \
  Building_data[cur_id]->housing.push_back( Citizen_amount((t), (n)) )

#define _military(n) \
  Building_data[cur_id]->military_support = (n)

#define _jobs(t, n) \
  Building_data[cur_id]->jobs = Citizen_amount((t), (n))

#define _wages(n) \
  Building_data[cur_id]->wages = (n)

#define _produces(t, n) \
  Building_data[cur_id]->add_production( (t), (n) )

// <recipes>

#define _recipe(r, a) \
  Building_data[cur_id]->recipes.push_back( Recipe( (r), (a) ) )

#define _units_per_day(a) \
  Building_data[cur_id]->recipes.back().units_per_day = (a); \
  Building_data[cur_id]->recipes.back().days_per_unit =  0

#define _days_per_unit(a) \
  Building_data[cur_id]->recipes.back().days_per_unit = (a); \
  Building_data[cur_id]->recipes.back().units_per_day =  0

#define _uses_resource(r, a) \
  Building_data[cur_id]->recipes.back().resource_ingredients.push_back( \
    Resource_amount( (r), (a) ) )

#define _uses_mineral(m, a) \
  Building_data[cur_id]->recipes.back().mineral_ingredients.push_back( \
    Mineral_amount( (m), (a) ) )

// </recipes>

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
    _name("hovels");
    _plural();
    _cost(RES_GOLD, 100);
    _cost(RES_WOOD, 10);
    _build_time(3);
    _destroy_cost(150);
    _upkeep(5);
    _housing(CIT_PEASANT, 100);

  _build(BUILD_HOUSE);
    _name("houses");
    _plural();
    _cost(RES_GOLD, 400);
    _cost(RES_WOOD, 20);
    _build_time(10);
    _destroy_cost(600);
    _upkeep(10);
    _housing(CIT_MERCHANT, 100);

  _build(BUILD_MANOR);
    _name("manor");
    _cost(RES_GOLD, 800);
    _cost(RES_STONE, 20);
    _build_time(30);
    _destroy_cost(1250);
    _upkeep(20);
    _housing(CIT_BURGHER, 50);

  _build(BUILD_KEEP);
    _name("keep");
    _cost(RES_GOLD, 1500);
    _cost(RES_STONE, 50);
    _build_time(90);
    _destroy_cost(2500);
    _upkeep(30);
    _housing(CIT_BURGHER, 5);
    _housing(CIT_MERCHANT, 50);
    _housing(CIT_PEASANT, 100);

  _build(BUILD_FARM);
    _name("farm");
    _cost(RES_GOLD, 100);
    _build_time(1);
    _destroy_cost(50);
    _upkeep(1);
    _jobs(CIT_PEASANT, 5);
    _wages(2);
    _produces(RES_FARMING, 10);

  _build(BUILD_QUARRY);
    _name("quarry");
    _cost(RES_GOLD, 500);
    _build_time(5);
    _destroy_cost(800);
    _upkeep(3);
    _jobs(CIT_PEASANT, 10);
    _wages(3);
    _produces(RES_STONE, 5);

  _build(BUILD_MINE);
    _name("mine");
    _cost(RES_GOLD, 500);
    _cost(RES_WOOD, 50);
    _build_time(10);
    _destroy_cost(1500);
    _upkeep(8);
    _jobs(CIT_PEASANT, 6);
    _wages(5);
    _produces(RES_MINING, 5);

  _build(BUILD_CAMP);
    _name("hunting camp");
    _cost(RES_GOLD, 50);
    _build_time(1);
    _destroy_cost(100);
    _jobs(CIT_PEASANT, 10);
    _wages(3);
    _produces(RES_HUNTING, 5);

  _build(BUILD_SAWMILL);
    _name("sawmill");
    _cost(RES_GOLD, 100);
    _build_time(5);
    _destroy_cost(200);
    _upkeep(3);
    _jobs(CIT_PEASANT, 5);
    _wages(3);
    _produces(RES_LOGGING, 5);

  _build(BUILD_BARRACKS);
    _name("barracks");
    _plural();
    _cost(RES_GOLD, 500);
    _cost(RES_WOOD, 40);
    _build_time(14);
    _destroy_cost(800);
    _upkeep(15);
    _military(50);

  _build(BUILD_MASONRY);
    _category(BUILDCAT_MANUFACTURING);
    _name("masonry");
    _cost(RES_GOLD, 150);
    _cost(RES_WOOD, 25);
    _build_time(5);
    _upkeep(8);
    _jobs(CIT_PEASANT, 3);
    _wages(5);
    _recipe(RES_STONE, 1);
      _units_per_day(3);
      _uses_mineral(MINERAL_STONE, 1);

  _build(BUILD_MINT);
    _category(BUILDCAT_MANUFACTURING);
    _name("mint");
    _cost(RES_GOLD, 150);
    _cost(RES_STONE, 15);
    _build_time(6);
    _upkeep(6);
    _jobs(CIT_MERCHANT, 3);
    _wages(5);
    _recipe(RES_GOLD, 1);
      _units_per_day(3);
      _uses_mineral(MINERAL_GOLD, 1);

}
