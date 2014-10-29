#ifndef _AI_CITY_H_
#define _AI_CITY_H_

#include "citizen.h"
#include "city.h"
#include "glyph.h"  // For AI_city::get_glyph()

class AI_city : public City
{
public:
  AI_city();
  virtual ~AI_city();

  virtual bool is_ai_city() { return true; }

// Mutators
  void set_name();

// Accessors
  int get_total_population(Citizen_type type = CIT_NULL);
  glyph get_glyph();

  std::string name;
  City_type type;
  Race race;

  Citizens population[CIT_MAX];

private:
  int resources[RES_MAX];
  int minerals[MINERAL_MAX];
};

#endif
