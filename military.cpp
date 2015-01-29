#include "military.h"
#include <sstream>

Military_unit::Military_unit()
{
  count = 0;
}

Military_unit::~Military_unit()
{
}

std::string Military_unit::save_data()
{
  std::stringstream ret;
  ret << count << " ";

  return ret.str();
}

bool Military_unit::load_data(std::istream& data)
{
  data >> count;

  return true;
}
