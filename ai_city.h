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
};

#endif
