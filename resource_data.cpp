#include "resource.h"

Crop_datum*     Crop_data   [CROP_MAX];
Mineral_datum*  Mineral_data[MINERAL_MAX];

#define _crop(n) \
  cur_id = (n)
#define _cname(s) \
  Crop_data[cur_id]->name = (s)
#define _cpercent(s) \
  Crop_data[cur_id]->percentage = (s)
#define _food(s) \
  Crop_data[cur_id]->food = (s)
#define _bonus(t, a) \
  Crop_data[cur_id]->bonus_resources.push_back( Resource_amount ( (t), (a) ) )
#define _type(s) \
  Crop_data[cur_id]->type = (s)

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


void init_crop_and_mineral_data()
{
  for (int i = 0; i < CROP_MAX; i++) {
    Crop_data[i] = new Crop_datum();
  }
  for (int i = 0; i < MINERAL_MAX; i++) {
    Mineral_data[i] = new Mineral_datum();
  }

// _food() is per 100 units of the crop!

  int cur_id = 0;

  _crop(CROP_NULL);
    _cname("any crop");

  _crop(CROP_WHEAT);
    _cname("wheat");
    _cpercent(90);
    _type(CROPTYPE_FOOD);
    _food(100);

  _crop(CROP_CABBAGE);
    _cname("cabbage");
    _cpercent(70);
    _type(CROPTYPE_FOOD);
    _food(120);

  _crop(CROP_GRAPES);
    _cname("grapes");
    _cpercent(30);
    _type(CROPTYPE_FOOD);
    _food(50);

  _crop(CROP_MELON);
    _cname("melon");
    _cpercent(40);
    _type(CROPTYPE_FOOD);
    _food(80);

  _crop(CROP_RICE);
    _cname("rice");
    _cpercent(90);
    _type(CROPTYPE_FOOD);
    _food(110);

  _crop(CROP_CACTUS);
    _cname("cactus");
    _cpercent(90);
    _type(CROPTYPE_FOOD);
    _food(60);

  _crop(CROP_PEPPER);
    _cname("pepper");
    _cpercent(60);
    _type(CROPTYPE_SPICE);
    _food(20);

  _crop(CROP_CINNAMON);
    _cname("cinnamon");
    _cpercent(40);
    _type(CROPTYPE_SPICE);

  _crop(CROP_TOBACCO);
    _cname("tobacco");
    _cpercent(30);
    _type(CROPTYPE_DRUG);

  _crop(CROP_AMANITAS);
    _cname("amanitas");
    _cpercent(10);
    _type(CROPTYPE_DRUG);

  _crop(CROP_AYAHUASCA);
    _cname("ayahuasca");
    _cpercent(10);
    _type(CROPTYPE_DRUG);

  _crop(CROP_SPICEREED);
    _cname("spicereed");
    _cpercent(8);
    _type(CROPTYPE_DRUG);

  _crop(CROP_DEATHCAP);
    _cname("deathcap");
    _cpercent(7);
    _type(CROPTYPE_POISON);

  _crop(CROP_VIPERVINE);
    _cname("vipervine");
    _cpercent(10);
    _type(CROPTYPE_POISON);

  _crop(CROP_SCORPICON);
    _cname("scorpicon");
    _cpercent(7);
    _type(CROPTYPE_POISON);

  _crop(CROP_COTTON);
    _cname("cotton");
    _cpercent(70);
    _type(CROPTYPE_FIBER);

  _crop(CROP_HEMP);
    _cname("hemp");
    _cpercent(60);
    _type(CROPTYPE_FIBER);

// Minerals

  _mineral(MINERAL_NULL);
    _mname("any mineral");
    _mcolor(c_dkgray);

  _mineral(MINERAL_STONE);
    _mname("stone");
    _mpercent(98);
    _mvalue(1);
    _mcolor(c_white);

  _mineral(MINERAL_TIN);
    _mname("tin");
    _mpercent(70);
    _mvalue(5);
    _mcolor(c_ltcyan);
    _mhidden();

  _mineral(MINERAL_COPPER);
    _mname("copper");
    _mpercent(50);
    _mvalue(20);
    _mcolor(c_ltred);
    _mhidden();

  _mineral(MINERAL_IRON);
    _mname("iron");
    _mpercent(70);
    _mvalue(25);
    _mcolor(c_cyan);
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
