#ifndef _AI_CITY_H_
#define _AI_CITY_H_

#include "citizen.h"
#include "city.h"
#include "glyph.h"  // For AI_city::get_glyph()
#include "ai.h"     // For City_role

class World_map;

class AI_city : public City
{
public:
  AI_city();
  virtual ~AI_city();

  virtual bool is_ai_city() { return true; }

// Randomize population, role, and more
  void randomize_properties(World_map* world);

private:
  City_role role;

};

#endif
