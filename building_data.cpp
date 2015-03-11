#include "building.h"
#include <sstream>

Building_datum* Building_data[BUILD_MAX];

#define _build(x) \
  cur_id = (x)

#define _name(n) \
  Building_data[cur_id]->name = (n)

#define _plural() \
  Building_data[cur_id]->plural = true

// Descriptions can use the <link=article>term</link> tags, to turn <term> into
// a link to the help article <article>
#define _description(n) \
  Building_data[cur_id]->description = (n)

#define _category(c) \
  Building_data[cur_id]->category = (c)

#define _forbidden(r) \
  Building_data[cur_id]->race_forbidden.push_back( (r) )

#define _built_by(r) \
  Building_data[cur_id]->race_can_build.push_back( (r) )

#define _cost(t, n) \
  Building_data[cur_id]->build_costs.push_back( Resource_amount( (t), (n) ) )

#define _unlock(t, a, b) \
  Building_data[cur_id]->unlockable = true; \
  Building_data[cur_id]->unlock_condition = City_achievement( (t), (a), (b) )

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

#define _livestock(n) \
  Building_data[cur_id]->livestock_space = (n)

#define _base_morale(n) \
  Building_data[cur_id]->base_morale = (n)

// <recipes>

#define _recipe(r, a) \
  Building_data[cur_id]->recipes.push_back( Recipe( (r), (a) ) )

#define _recipe_name(n) \
  Building_data[cur_id]->recipes.back().name = (n)

#define _units_per_day(a) \
  Building_data[cur_id]->recipes.back().units_per_day = (a); \
  Building_data[cur_id]->recipes.back().days_per_unit =  0

#define _days_per_unit(a) \
  Building_data[cur_id]->recipes.back().days_per_unit = (a); \
  Building_data[cur_id]->recipes.back().units_per_day =  0

#define _max_deficit(a) \
  Building_data[cur_id]->recipes.back().max_deficit = (a)

#define _uses_resource(r, a) \
  Building_data[cur_id]->recipes.back().resource_ingredients.push_back( \
    Resource_amount( (r), (a) ) )

#define _uses_mineral(m, a) \
  Building_data[cur_id]->recipes.back().mineral_ingredients.push_back( \
    Mineral_amount( (m), (a) ) )

// </recipes>


void init_building_data()
{
  for (int i = 0; i < BUILD_MAX; i++) {
    Building_data[i] = new Building_datum;
    Building_data[i]->uid = i;
  }

  int cur_id;

  _build(BUILD_PARK);
    _name("park");
    _cost(RES_GOLD, 300);
    _build_time(3);
    _destroy_cost(20);
    _upkeep(4);
    _base_morale(3);
    _description("\
A park is a well-groomed area of nature, with carefully designed paths and \
seating areas.\
");

  _build(BUILD_PLAZA);
    _name("plaza");
    _cost(RES_GOLD,  500);
    _cost(RES_STONE, 500);
    _build_time(5);
    _destroy_cost(150);
    _upkeep(4);
    _base_morale(1);
    _description("\
A plaza is an urban area that combines the pleasant environment and seating \
and strolling areas of a <link=park>park</link>, with the \
<link=building>business</link> space of a <link=marketplace>marketplace</link>.\
");

  _build(BUILD_MARKETPLACE);
    _name("marketplace");
    _cost(RES_GOLD,   800);
    _cost(RES_STONE, 1500);
    _build_time(7);
    _destroy_cost(350);
    _upkeep(2);
    _description("\
A marketplace is an area designed purely for business, with lots of space for \
<link=building>buildings</link> of various natures.\
");

  _build(BUILD_HOVEL);
    _name("hovels");
    _plural();
    _cost(RES_GOLD, 300);
    _cost(RES_WOOD, 300);
    _build_time(3);
    _destroy_cost(250);
    _upkeep(5);
    _housing(CIT_PEASANT, 100);
    _description("\
Hovels are simple homes, sacrificing comfort and style to be a purely \
utilitarian living space for <link=peasants>peasants</link>.\
");

  _build(BUILD_HOUSE);
    _name("houses");
    _plural();
    _cost(RES_GOLD, 800);
    _cost(RES_WOOD, 500);
    _build_time(10);
    _destroy_cost(600);
    _upkeep(10);
    _housing(CIT_MERCHANT, 100);
    _description("\
Houses are respectable homes with a moderate degree of comfort, suitable for \
the <link=merchants>middle classes</link>.\
");

  _build(BUILD_MANOR);
    _name("manor");
    _cost(RES_GOLD,  1200);
    _cost(RES_WOOD,   500);
    _cost(RES_STONE, 1000);
    _build_time(30);
    _destroy_cost(1250);
    _upkeep(20);
    _housing(CIT_BURGHER, 50);
    _description("\
A manor is a large, well-furnished home.  Its high degree of comfort and low-\
density quarters are ideal for the <link=burghers>upper class</link>.\
");

  _build(BUILD_KEEP);
    _name("keep");
    _cost(RES_GOLD,  3000);
    _cost(RES_STONE, 4000);
    _build_time(90);
    _destroy_cost(2500);
    _upkeep(30);
    _housing(CIT_BURGHER,   5);
    _housing(CIT_MERCHANT, 50);
    _housing(CIT_PEASANT, 100);
    _description("\
A keep is the center of any city.  It provides a home for the \
<link=nobles>nobility</link> as well as space for some <link=citizens>citizens\
</link>.  It also has space to support several <link=building>businesses\
</link>.\
");

  _build(BUILD_FARM);
    _name("farm");
    _cost(RES_GOLD, 250);
    _build_time(1);
    _destroy_cost(50);
    _upkeep(1);
    _jobs(CIT_PEASANT, 15);
    _wages(1);
    _produces(RES_FARMING, 1);
    _description("\
A farm is a tilled piece of land designed for growing <link=crop>crops</link>.\
");

  _build(BUILD_HUNTING_CAMP);
    _name("hunting camp");
    _cost(RES_GOLD, 200);
    _build_time(1);
    _destroy_cost(100);
    _upkeep(3);
    _jobs(CIT_PEASANT, 10);
    _wages(1);
    _produces(RES_HUNTING, 1);
    _description("\
A hunting camp is a base of operations for <link=hunting>hunters</link>, \
allowing them to catch <link=animals>game</link> in the land upon which it is \
built.\
");

  _build(BUILD_MINE);
    _name("mine");
    _cost(RES_GOLD, 500);
    _cost(RES_WOOD, 400);
    _build_time(10);
    _destroy_cost(1500);
    _upkeep(8);
    _jobs(CIT_PEASANT, 8);
    _wages(2);
    _produces(RES_MINING, 5);
    _description("\
A mine is set of several shafts dug into the land, allowing <link=mining>miners\
</link> to pull <link=minerals>minerals</link> from the ground.\
");

  _build(BUILD_SAWMILL);
    _name("sawmill");
    _forbidden(RACE_ELF);
    _cost(RES_GOLD,  200);
    _cost(RES_STONE, 350);
    _build_time(5);
    _destroy_cost(350);
    _upkeep(3);
    _jobs(CIT_PEASANT, 6);
    _wages(1);
    _produces(RES_LOGGING, 5);
    _description("\
A sawmill is a site for the processing of <link=logging>logging</link> \
operations.  It should be built on a <link=city map tile>map tile</link> that \
has <link=trees>trees</link> growing on it; it will remove the trees and turn \
them into <link=wood>wood</link> for use.  Once the trees are cleared, the \
sawmill will automatically close.\
");

  _build(BUILD_PASTURE);
    _name("pasture");
    _cost(RES_GOLD, 100);
    _cost(RES_WOOD, 100);
    _build_time(1);
    _destroy_cost(20);
    _upkeep(1);
    _livestock(500);
    _description("\
A pasture is a fenced-in area used for keeping <link=livestock>livestock\
</link>.  The terrain it is built upon does not matter.\
");

  _build(BUILD_BARRACKS);
    _name("barracks");
    _plural();
    _cost(RES_GOLD, 800);
    _cost(RES_WOOD, 650);
    _build_time(14);
    _destroy_cost(800);
    _upkeep(15);
    _military(50);
    _description("\
Barracks are a space for the housing and training of <link=army>military units\
</link>.\
");

  _build(BUILD_MASONRY);
    _category(BUILDCAT_MANUFACTURING);
    _name("masonry");
    _cost(RES_GOLD, 250);
    _cost(RES_WOOD, 600);
    _build_time(5);
    _upkeep(3);
    _jobs(CIT_PEASANT, 3);
    _wages(1);
    _recipe(RES_STONE, 1);
      _units_per_day(3);
      _max_deficit(0);
      _uses_mineral(MINERAL_STONE, 1);
    _description("\
A masonry is a <link=building>building</link> where uncut <link=stone>stone\
</link> is cut into useful blocks.\
");

  _build(BUILD_SMELTERY);
    _category(BUILDCAT_MANUFACTURING);
    _name("smeltery");
    _cost(RES_GOLD,   500);
    _cost(RES_STONE, 1000);
    _build_time(6);
    _unlock(CITY_ACHIEVE_ORES, 0, 0);
    _jobs(CIT_PEASANT, 10);
    _wages(2);
    _recipe(RES_TIN, 1);
      _recipe_name("Smelt tin (burn wood)");
      _units_per_day(1);
      _max_deficit(0);
      _uses_mineral(MINERAL_TIN, 1);
      _uses_resource(RES_WOOD, 3);
    _recipe(RES_TIN, 3);
      _recipe_name("Smelt tin (burn coal)");
      _units_per_day(3);
      _max_deficit(0);
      _uses_mineral(MINERAL_TIN, 3);
      _uses_mineral(MINERAL_COAL, 1);
    _recipe(RES_COPPER, 1);
      _recipe_name("Smelt copper (burn wood)");
      _units_per_day(1);
      _max_deficit(0);
      _uses_mineral(MINERAL_COPPER, 1);
      _uses_resource(RES_WOOD, 3);
    _recipe(RES_COPPER, 3);
      _recipe_name("Smelt copper (burn coal)");
      _units_per_day(3);
      _max_deficit(0);
      _uses_mineral(MINERAL_COPPER, 3);
      _uses_mineral(MINERAL_COAL, 1);
    _recipe(RES_IRON, 1);
      _recipe_name("Smelt iron (burn wood)");
      _days_per_unit(2);
      _max_deficit(0);
      _uses_mineral(MINERAL_IRON, 1);
      _uses_resource(RES_WOOD, 3);
    _recipe(RES_IRON, 3);
      _recipe_name("Smelt iron (burn coal)");
      _units_per_day(2);
      _max_deficit(0);
      _uses_mineral(MINERAL_IRON, 3);
      _uses_mineral(MINERAL_COAL, 1);
    _description("\
A smeltery is a high-temperature furnace used to melt metals out of the \
<link=minerals>ores</link> they naturally appear in.  These ores are virtually \
useless before smelting; thus a smeltery is a vital addition to any mining \
operation.  It is possible to smelt ores using <link=wood>wood</link> as a \
fuel; however, it is much faster and more efficient to burn <link=coal>coal\
</link> if it is available.\
");

  _build(BUILD_MINT);
    _category(BUILDCAT_MANUFACTURING);
    _name("mint");
    _cost(RES_GOLD,   500);
    _cost(RES_STONE, 1000);
    _cost(RES_IRON,   200);
    _build_time(6);
    _unlock( CITY_ACHIEVE_POP, CIT_MERCHANT, 1 );
    _upkeep(6);
    _jobs(CIT_MERCHANT, 3);
    _wages(4);
    _recipe(RES_GOLD, 1);
      _recipe_name("Gold (burn wood)");
      _units_per_day(3);
      _max_deficit(0);
      _uses_mineral(MINERAL_GOLD, 1);
      _uses_resource(RES_WOOD, 3);
    _recipe(RES_GOLD, 3);
      _recipe_name("Gold (burn coal)");
      _units_per_day(3);
      _max_deficit(0);
      _uses_mineral(MINERAL_GOLD, 3);
      _uses_mineral(MINERAL_COAL, 1);
    _description("\
A mint is a <link=building>building</link> where raw <link=gold>gold</link> \
ore can be forged into spendable coins and bars.  Doing so requires a fuel \
source; <link=wood>wood</link> can be used, but <link=coal>coal</link> is more \
efficient.\
");

  _build(BUILD_KITCHEN);
    _category(BUILDCAT_MANUFACTURING);
    _name("kitchen");
    _cost(RES_GOLD,  400);
    _cost(RES_WOOD,  400);
    _cost(RES_STONE, 800);
    _build_time(4);
    _unlock( CITY_ACHIEVE_POP, CIT_MERCHANT, 1 );
    _upkeep(3);
    _jobs(CIT_MERCHANT, 5);
    _wages(3);
    _recipe(RES_SALT, 1);
      _recipe_name("Prepare salt");
      _units_per_day(5);
      _max_deficit(0);
      _uses_mineral(MINERAL_SALT, 1);
    _description("\
A kitchen is a <link=building>building</link> where food and spices can be \
processed.  Notably, raw <link=salt>salt</link> can be processed into a \
useable <link=luxury>luxury</link>.\
");

  _build(BUILD_TAILOR);
    _category(BUILDCAT_MANUFACTURING);
    _name("tailor");
    _cost(RES_GOLD,  500);
    _cost(RES_WOOD,  500);
    _cost(RES_STONE, 500);
    _build_time(3);
    _unlock( CITY_ACHIEVE_POP, CIT_MERCHANT, 1 );
    _upkeep(2);
    _jobs(CIT_MERCHANT, 5);
    _wages(5);
    _recipe(RES_CLOTHING, 1);
      _recipe_name("Sew clothing");
      _units_per_day(3);
      _max_deficit(-10);
      _uses_resource(RES_FIBER, 10);
    _recipe(RES_FUR_COATS, 1);
      _recipe_name("Sew fur coats");
      _units_per_day(2);
      _max_deficit(-8);
      _uses_resource(RES_FUR,    8);
    _recipe(RES_LEATHER_COATS, 1);
      _recipe_name("Sew leather coats");
      _units_per_day(2);
      _uses_resource(RES_LEATHER, 8);
    _description("\
A tailor is a <link=building>building</link> where <link=clothing>clothing\
</link> and coats (either <link=fur coats>fur</link> or \
<link=leather coats>leather</link>) can be sewn.\
");

  _build(BUILD_WOOD_SHOP);
    _category(BUILDCAT_MANUFACTURING);
    _name("woodworking shop");
    _cost(RES_GOLD,  500);
    _cost(RES_WOOD, 1000);
    _cost(RES_IRON,  150);
    _build_time(4);
    _unlock( CITY_ACHIEVE_POP, CIT_MERCHANT, 1 );
    _upkeep(2);
    _jobs(CIT_MERCHANT, 5);
    _wages(5);
    _recipe(RES_WOOD_HOUSEWARES, 1);
      _recipe_name("Wooden housewares");
      _units_per_day(4);
      _max_deficit(-15);
      _uses_resource(RES_WOOD, 3);
    _recipe(RES_FURNITURE, 1);
      _recipe_name("Furniture");
      _days_per_unit(2);
      _max_deficit(-25);
      _uses_resource(RES_WOOD, 15);
    _description("\
A woodworking shop is a <link=building>building</link> where woodworkers are \
employed, making a variety of goods from <link=wood>wood</link>.\
");

  _build(BUILD_SMITH);
    _category(BUILDCAT_MANUFACTURING);
    _name("smith");
    _cost(RES_GOLD,   750);
    _cost(RES_STONE, 1000);
    _cost(RES_IRON,   750);
    _build_time(5);
    _unlock( CITY_ACHIEVE_POP, CIT_MERCHANT, 1 );
    _upkeep(3);
    _jobs(CIT_MERCHANT, 6);
    _wages(5);
    _recipe(RES_TIN_HOUSEWARES, 1);
      _recipe_name("Smith tin housewares");
      _units_per_day(3);
      _max_deficit(-15);
      _uses_resource(RES_TIN, 3);
    _recipe(RES_COPPER_HOUSEWARES, 1);
      _recipe_name("Smith copper housewares");
      _units_per_day(3);
      _max_deficit(-15);
      _uses_resource(RES_COPPER, 3);
    _recipe(RES_IRON_HOUSEWARES, 1);
      _recipe_name("Smith iron housewares");
      _units_per_day(3);
      _max_deficit(-15);
      _uses_resource(RES_IRON, 3);
    _recipe(RES_JEWELRY, 1);
      _recipe_name("Craft jewelry");
      _days_per_unit(2);
      _max_deficit(-3);
      _uses_resource(RES_TIN,      1);
      _uses_resource(RES_COPPER,   1);
      _uses_mineral (MINERAL_GEMS, 2);
    _description("\
A smith is a <link=building>building</link> where various metal goods are \
fashioned.\
");

}
