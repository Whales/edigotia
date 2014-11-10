#ifndef _AI_H_
#define _AI_H_

// City_specialization is an attempt to abstract what makes an AI city
// "special."  It primarily affects their imports and exports, but may also
// affect actions as well.

enum City_specialization
{
  CITYSPEC_NULL = 0,    // No specialization

  CITYSPEC_FOOD,        // Food production - exports lots of food
  CITYSPEC_MINING,      // Mining - exports ores

  CITYSPEC_MAX
};

#endif
