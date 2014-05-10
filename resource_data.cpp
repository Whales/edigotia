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
#define _type(s) \
  Drop_data[cur_id]->type = (s)

#define _mineral(n) \
  cur_id = (n)
#define _mpercent(s) \
  Mineral_data[cur_id]->percentage = (s)
#define _mname(s) \
  Mineral_data[cur_id]->name = (s)


void init_crops_and_minerals()
{
  for (int i = 0; i < CROP_MAX; i++) {
    Crop_data[i] = new Crop_datum();
  }
  for (int i = 0; i < MINERAL_MAX; i++) {
    Mineral_data[i] = new Mineral_datum();
  }

  int cur_id = 0;

  _crop(CROP_WHEAT);
    _cname("wheat");
    _cpercent(100);
    _type(CROPTYPE_FOOD);
    _food(10);

  _crop(CROP_CORN);
    _cname("corn");
    _cpercent(90);
    _type(CROPTYPE_FOOD);
    _food(10);

  _crop(CROP_CABBAGE);
    _cname("cabbage");
    _cpercent(80);
    _type(CROPTYPE_FOOD);
    _food(12);

  _crop(CROP_GRAPES);
    _cname("grapes");
    _cpercent(35);
    _type(CROPTYPE_FOOD);
    _food(5);

  _crop(CROP_MELON);
    _cname("melon");
    _cpercent(50);
    _type(CROPTYPE_FOOD);
    _food(8);

  _crop(CROP_RICE);
    _cname("rice");
    _cpercent(100);
    _type(CROPTYPE_FOOD);
    _food(10);

  _crop(CROP_CACTUS);
    _cname("cactus");
    _cpercent(90);
    _type(CROPTYPE_FOOD);
    _food(6);

  _crop(CROP_PEPPER);
    _cname("pepper");
    _cpercent(80);
    _type(CROPTYPE_SPICE);
    _food(2);

  _crop(CROP_CINNAMON);
    _cname("cinnamon");
    _cpercent(60);
    _type(CROPTYPE_SPICE);

  _crop(CROP_TOBACCO);
    _cname("tobacco");
    _cpercent(40);
    _type(CROPTYPE_DRUG);

  _crop(CROP_AMANITAS);
    _cname("amanitas");
    _cpercent(20);
    _type(CROPTYPE_DRUG);

  _crop(CROP_AYAHUASCA);
    _cname("ayahuasca");
    _cpercent(20);
    _type(CROPTYPE_DRUG);

  _crop(CROP_SPICEREED);
    _cname("spicereed");
    _cpercent(10);
    _type(CROPTYPE_DRUG);

  _crop(CROP_DEATHCAP);
    _cname("deathcap");
    _cpercent(10);
    _type(CROPTYPE_POISON);

  _crop(CROP_VIPERVINE);
    _cname("vipervine");
    _cpercent(25);
    _type(CROPTYPE_POISON);

  _crop(CROP_SCORPICON);
    _cname("scorpicon");
    _cpercent(10);
    _type(CROPTYPE_POISON);

  _crop(CROP_COTTON);
    _cname("cotton");
    _cpercent(80);
    _type(CROPTYPE_MATERIAL);

  _crop(CROP_HEMP);
    _cname("hemp");
    _cpercent(70);
    _type(CROPTYPE_MATERIAL);

  _crop(CROP_HAY);
    _cname("hay");
    _cpercent(100);
    _type(CROPTYPE_OTHER);


// Minerals

  _mineral(MINERAL_STONE);
    _mname("stone");

  _mineral(MINERAL_TIN);
    _mname("tin");

  _mineral(MINERAL_COPPER);
    _mname("copper");

  _mineral(MINERAL_IRON);
    _mname("iron");

  _mineral(MINERAL_GOLD);
    _mname("gold");

  _mineral(MINERAL_COAL);
    _mname("coal");
}
