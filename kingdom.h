#ifndef _KINGDOM_H_
#define _KINGDOM_H_

#include "city.h"
#include "color.h"
//#include "race.h"

class Kingdom
{
public:
  Kingdom();
  ~Kingdom();

  //Race race;
  City capitol;
  nc_color color;

  std::vector<City> dukes;
};

#endif
