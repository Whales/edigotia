#ifndef _MILITARY_H_
#define _MILITARY_H_

#include <string>
#include <istream>

struct Military_unit
{
  Military_unit();
  ~Military_unit();

  std::string save_data();
  bool load_data(std::istream& data);

  int count;
};

#endif
