#include "resource.h"

Resource_datum* Resource_data [RES_MAX];
Crop_datum*     Crop_data     [CROP_MAX];
Mineral_datum*  Mineral_data  [MINERAL_MAX];

// RESOURCE MACROS
#define _resource(n) \
  cur_id = (n)
#define _rname(s) \
  Resource_data[cur_id]->name = (s)
#define _rcolor(c) \
  Resource_data[cur_id]->color = (c)
#define _rvalue(n) \
  Resource_data[cur_id]->value = (n)
#define _luxury(t) \
  Resource_data[cur_id]->luxury_type = (t)
#define _demand(n) \
  Resource_data[cur_id]->demand = (n)
#define _morale(n) \
  Resource_data[cur_id]->morale = (n)
#define _meta() \
  Resource_data[cur_id]->meta = true

// CROP MACROS
#define _crop(n) \
  cur_id = (n)
#define _cname(s) \
  Crop_data[cur_id]->name = (s)
// percentage is the frequency of placement; what % of terrain should it cover
#define _cpercent(s) \
  Crop_data[cur_id]->percentage = (s)
#define _temperature(a, b) \
  Crop_data[cur_id]->min_temp = (a); \
  Crop_data[cur_id]->max_temp = (b)
#define _altitude(a, b) \
  Crop_data[cur_id]->min_altitude = (a); \
  Crop_data[cur_id]->max_altitude = (b)
#define _rainfall(a, b) \
  Crop_data[cur_id]->min_rainfall = (a); \
  Crop_data[cur_id]->max_rainfall = (b)
#define _food(s) \
  Crop_data[cur_id]->food = (s)
#define _bonus(t, a) \
  Crop_data[cur_id]->bonus_resources.push_back( Resource_amount ( (t), (a) ) )
#define _type(s) \
  Crop_data[cur_id]->type = (s)

// MINERAL MACROS
#define _mineral(n) \
  cur_id = (n)
#define _mpercent(s) \
  Mineral_data[cur_id]->percentage = (s)
#define _mvalue(s) \
  Mineral_data[cur_id]->value = (s)
#define _mname(s) \
  Mineral_data[cur_id]->name = (s)
#define _mcolor(c) \
  Mineral_data[cur_id]->color = (c)
#define _mhidden() \
  Mineral_data[cur_id]->hidden = true


void init_resource_data()
{
  for (int i = 0; i < RES_MAX; i++) {
    Resource_data[i] = new Resource_datum();
  }
  for (int i = 0; i < CROP_MAX; i++) {
    Crop_data[i] = new Crop_datum();
  }
  for (int i = 0; i < MINERAL_MAX; i++) {
    Mineral_data[i] = new Mineral_datum();
  }


  int cur_id = 0;

  _resource(RES_NULL);
    _rname("any resource");
    _rcolor(c_dkgray);

  _resource(RES_GOLD);
    _rname("gold");
    _rvalue(100);
    _rcolor(c_yellow);

  _resource(RES_FOOD);
    _rname("food");
    _rvalue(20);
    _rcolor(c_green);

  _resource(RES_WOOD);
    _rname("wood");
    _rvalue(30);
    _rcolor(c_ltred);

  _resource(RES_STONE);
    _rname("stone");
    _rvalue(10);
    _rcolor(c_ltgray);

  _resource(RES_TIN);
    _rname("tin");
    _rvalue(15);
    _rcolor(c_ltcyan);

  _resource(RES_COPPER);
    _rname("copper");
    _rvalue(30);
    _rcolor(c_ltred);

  _resource(RES_IRON);
    _rname("iron");
    _rvalue(50);
    _rcolor(c_cyan);

  _resource(RES_FIBER);
    _rname("fiber");
    _rvalue(14);
    _rcolor(c_ltgray);

  _resource(RES_FUR);
    _rname("fur");
    _rvalue(16);
    _rcolor(c_brown);

  _resource(RES_LEATHER);
    _rname("leather");
    _rvalue(16);
    _rcolor(c_brown);

  _resource(RES_UNICORN_HORN);
    _rname("unicorn horn");
    _rvalue(5000);
    _rcolor(c_pink);

  _resource(RES_SALT);
    _rname("salt");
    _rvalue(25);
    _rcolor(c_white);
    _luxury(LUX_NULL);  // Salt's technical a spice, but it's universally wanted
    _demand(100);
    _morale(6);

  _resource(RES_PEPPER);
    _rname("pepper");
    _rvalue(28);
    _rcolor(c_dkgray);
    _luxury(LUX_SPICE);
    _demand(80);
    _morale(5);

  _resource(RES_CINNAMON);
    _rname("cinnamon");
    _rvalue(26);
    _rcolor(c_brown);
    _luxury(LUX_SPICE);
    _demand(40);
    _morale(3);

  _resource(RES_CUMIN);
    _rname("cumin");
    _rvalue(45);
    _rcolor(c_brown);
    _luxury(LUX_SPICE);
    _demand(30);
    _morale(5);

  _resource(RES_PAPRIKA);
    _rname("paprika");
    _rvalue(20);
    _rcolor(c_ltred);
    _luxury(LUX_SPICE);
    _demand(60);
    _morale(3);

  _resource(RES_TOBACCO);
    _rname("tobacco");
    _rvalue(30);
    _rcolor(c_brown);
    _luxury(LUX_SMOKABLE);
    _demand(75);
    _morale(5);

  _resource(RES_CANNABIS);
    _rname("cannabis");
    _rvalue(38);
    _rcolor(c_green);
    _luxury(LUX_SMOKABLE);
    _demand(40);
    _morale(5);

  _resource(RES_AMANITAS);
    _rname("amanitas");
    _rvalue(75);
    _rcolor(c_magenta);
    _luxury(LUX_HALLUCINOGEN);
    _demand(5);
    _morale(3);

  _resource(RES_AYAHUASCA);
    _rname("ayahuasca");
    _rvalue(80);
    _rcolor(c_magenta);
    _luxury(LUX_HALLUCINOGEN);
    _demand(4);
    _morale(3);

  _resource(RES_SPICEREED);
    _rname("spicereed");
    _rvalue(45);
    _rcolor(c_magenta);
    _luxury(LUX_HALLUCINOGEN);
    _demand(30);
    _morale(4);

  _resource(RES_CLOTHING);
    _rname("clothing");
    _rvalue(35);
    _rcolor(c_white);
    _demand(20);
    _morale(2);

  _resource(RES_FUR_COATS);
    _rname("fur coats");
    _rvalue(60);
    _rcolor(c_brown);
    _luxury(LUX_COATS);
    _demand(10);
    _morale(3);

  _resource(RES_LEATHER_COATS);
    _rname("leather coats");
    _rvalue(65);
    _rcolor(c_brown);
    _luxury(LUX_COATS);
    _demand(8);
    _morale(3);

  _resource(RES_WOOD_HOUSEWARES);
    _rname("wood housewares");
    _rvalue(70);
    _rcolor(c_ltred);
    _luxury(LUX_HOUSEWARES);
    _demand(10);
    _morale(4);

  _resource(RES_TIN_HOUSEWARES);
    _rname("tin housewares");
    _rvalue(85);
    _rcolor(c_ltcyan);
    _luxury(LUX_HOUSEWARES);
    _demand(10);
    _morale(5);

  _resource(RES_COPPER_HOUSEWARES);
    _rname("copper housewares");
    _rvalue(90);
    _rcolor(c_ltred);
    _luxury(LUX_HOUSEWARES);
    _demand(10);
    _morale(6);

  _resource(RES_IRON_HOUSEWARES);
    _rname("iron housewares");
    _rvalue(110);
    _rcolor(c_cyan);
    _luxury(LUX_HOUSEWARES);
    _demand(10);
    _morale(7);

  _resource(RES_FURNITURE);
    _rname("furniture");
    _rvalue(130);
    _rcolor(c_ltred);
    _demand(8);
    _morale(8);

  _resource(RES_JEWELRY);
    _rname("jewelry");
    _rvalue(200);
    _rcolor(c_magenta);
    _demand(5);
    _morale(12);

  _resource(RES_FARMING);
    _rname("farming");
    _rcolor(c_magenta);
    _meta();

  _resource(RES_MINING);
    _rname("mining");
    _rcolor(c_magenta);
    _meta();

  _resource(RES_HUNTING);
    _rname("hunting");
    _rcolor(c_magenta);
    _meta();

  _resource(RES_LOGGING);
    _rname("logging");
    _rcolor(c_magenta);
    _meta();

// _food() is per 100 units of the crop!
  _crop(CROP_NULL);
    _cname("any crop");

  _crop(CROP_WHEAT);
    _cname("wheat");
    _cpercent(90);
    _temperature(25, 90);
    _altitude(0, 90);
    _rainfall(10, 70);
    _type(CROPTYPE_FOOD);
    _food(100);

  _crop(CROP_CABBAGE);
    _cname("cabbage");
    _cpercent(70);
    _temperature(10, 75);
    _altitude(0, 80);
    _rainfall(5, 100);
    _type(CROPTYPE_FOOD);
    _food(120);

  _crop(CROP_GRAPES);
    _cname("grapes");
    _cpercent(30);
    _temperature(30, 70);
    _altitude(15, 90);
    _rainfall(15, 60);
    _type(CROPTYPE_FOOD);
    _food(50);

  _crop(CROP_MELON);
    _cname("melon");
    _cpercent(40);
    _temperature(35, 90);
    _altitude(0, 60);
    _rainfall(20, 100);
    _type(CROPTYPE_FOOD);
    _food(80);

  _crop(CROP_RICE);
    _cname("rice");
    _cpercent(90);
    _temperature(50, 100);
    _altitude(0, 45);
    _rainfall(45, 100);
    _type(CROPTYPE_FOOD);
    _food(110);

  _crop(CROP_CACTUS);
    _cname("cactus");
    _cpercent(90);
    _temperature(60, 100);
    _altitude(0, 80);
    _rainfall(0, 15);
    _type(CROPTYPE_FOOD);
    _food(60);
    _bonus(RES_FIBER, 20);

  _crop(CROP_PEPPER);
    _cname("pepper");
    _cpercent(40);
    _temperature(60, 90);
    _altitude(0, 60);
    _rainfall(10, 60);
    _type(CROPTYPE_SPICE);
    _food(15);
    _bonus(RES_PEPPER, 90);

  _crop(CROP_CINNAMON);
    _cname("cinnamon");
    _cpercent(20);
    _temperature(65, 90);
    _altitude(15, 80);
    _rainfall(10, 60);
    _type(CROPTYPE_SPICE);
    _bonus(RES_CINNAMON, 100);

  _crop(CROP_CUMIN);
    _cname("cumin");
    _cpercent(8);
    _temperature(70, 100);
    _altitude(10, 65);
    _rainfall(40, 100);
    _type(CROPTYPE_SPICE);
    _bonus(RES_CUMIN, 80);

  _crop(CROP_PAPRIKA);
    _cname("paprika");
    _cpercent(45);
    _temperature(40, 70);
    _altitude(0, 50);
    _rainfall(20, 60);
    _type(CROPTYPE_SPICE);
    _bonus(RES_PAPRIKA, 100);

  _crop(CROP_TOBACCO);
    _cname("tobacco");
    _cpercent(30);
    _temperature(50, 85);
    _altitude(0, 50);
    _rainfall(20, 50);
    _type(CROPTYPE_DRUG);
    _bonus(RES_TOBACCO, 100);

  _crop(CROP_AMANITAS);
    _cname("amanitas");
    _cpercent(10);
    _temperature(0, 40);
    _altitude(0, 70);
    _rainfall(20, 100);
    _type(CROPTYPE_DRUG);
    _bonus(RES_AMANITAS, 100);

  _crop(CROP_AYAHUASCA);
    _cname("ayahuasca");
    _cpercent(10);
    _temperature(65, 100);
    _altitude(20, 70);
    _rainfall(60, 100);
    _type(CROPTYPE_DRUG);
    _bonus(RES_AYAHUASCA, 100);

  _crop(CROP_SPICEREED);
    _cname("spicereed");
    _cpercent(8);
    _temperature(70, 100);
    _altitude(0, 30);
    _rainfall(0, 15);
    _type(CROPTYPE_DRUG);
    _bonus(RES_SPICEREED, 100);

  _crop(CROP_DEATHCAP);
    _cname("deathcap");
    _cpercent(7);
    _temperature(20, 70);
    _altitude(0, 70);
    _rainfall(45, 100);
    _type(CROPTYPE_POISON);

  _crop(CROP_VIPERVINE);
    _cname("vipervine");
    _cpercent(10);
    _temperature(65, 100);
    _altitude(20, 70);
    _rainfall(40, 100);
    _type(CROPTYPE_POISON);

  _crop(CROP_SCORPICON);
    _cname("scorpicon");
    _cpercent(7);
    _temperature(60, 100);
    _altitude(0, 70);
    _rainfall(0, 10);
    _type(CROPTYPE_POISON);

  _crop(CROP_COTTON);
    _cname("cotton");
    _cpercent(70);
    _temperature(60, 90);
    _altitude(0, 80);
    _rainfall(10, 50);
    _type(CROPTYPE_FIBER);
    _bonus(RES_FIBER, 100);

  _crop(CROP_HEMP);
    _cname("hemp");
    _cpercent(60);
    _temperature(25, 90);
    _altitude(0, 100);
    _rainfall(5, 100);
    _type(CROPTYPE_FIBER);
    _bonus(RES_FIBER, 80);
    _bonus(RES_CANNABIS, 20);

// Minerals

  _mineral(MINERAL_NULL);
    _mname("any mineral");
    _mcolor(c_dkgray);

  _mineral(MINERAL_STONE);
    _mname("stone");
    _mpercent(98);
    _mvalue(5);
    _mcolor(c_white);

  _mineral(MINERAL_TIN);
    _mname("tin ore");
    _mpercent(70);
    _mvalue(5);
    _mcolor(c_ltcyan);
    _mhidden();

  _mineral(MINERAL_COPPER);
    _mname("copper ore");
    _mpercent(50);
    _mvalue(10);
    _mcolor(c_ltred);
    _mhidden();

  _mineral(MINERAL_IRON);
    _mname("iron ore");
    _mpercent(70);
    _mvalue(30);
    _mcolor(c_cyan);
    _mhidden();

  _mineral(MINERAL_SALT);
    _mname("salt");
    _mpercent(50);
    _mvalue(28);
    _mcolor(c_white);
    _mhidden();

  _mineral(MINERAL_GEMS);
    _mname("gems");
    _mpercent(3);
    _mvalue(100);
    _mcolor(c_pink);
    _mhidden();

// The value of gold is less than 100 because it takes labor and fuel to
// convert it into spendable currency!
  _mineral(MINERAL_GOLD);
    _mname("gold");
    _mpercent(4);
    _mvalue(80);
    _mcolor(c_yellow);
    _mhidden();

  _mineral(MINERAL_COAL);
    _mname("coal");
    _mpercent(20);
    _mvalue(50);
    _mcolor(c_dkgray);
    _mhidden();
}
