#ifndef _AI_H_
#define _AI_H_

#include "resource.h"
#include <vector>


// City_specialization is an attempt to abstract what makes an AI city
// "special."  It primarily affects their imports and exports, but may also
// affect actions as well.

enum City_specialization
{
  CITYSPEC_NULL = 0,    // No specialization

  CITYSPEC_FARMING,     // Food production - exports lots of food
  CITYSPEC_MINING,      // Mining - exports ores

  CITYSPEC_MAX
};

struct City_spec_datum
{
  std::string name;

  std::vector<Resource> res_exports;
  std::vector<Resource> res_imports;
  std::vector<Mineral>  min_exports;
  std::vector<Mineral>  min_imports;
};

// These are in ai_data.cpp
City_spec_datum* City_spec_data[CITYSPEC_MAX];
void init_city_specializiations();

#endif
