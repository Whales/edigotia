#include "resource.h"

Crop_datum*     Crop_data   [CROP_MAX];
Mineral_datum*  Mineral_data[MINERAL_MAX];

#define _crop(n) \
  cur_id = (n)
#define _cname(s) \
  Crop_data[cur_id]->name = (s)
#define _food(s) \
  Crop_data[cur_id]->food = (s)
#define _type(s) \
  Drop_data[cur_id]->type = (s)

#define _mineral(n) \
  cur_id = (n)
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
    _name("wheat");
    _type(CROPTYPE_FOOD);
    _food(10);
