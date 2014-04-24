#include "city.h"
#include "cuss.h"
#include "window.h"

City::City()
{
  population[CIT_NULL]     =   0;
  population[CIT_SLAVE]    =   0;
  population[CIT_PEASANT]  = 100;
  population[CIT_MERCHANT] =   0;
  population[CIT_BURGHER]  =   0;
  population[CIT_NOBLE]    =   0;

  for (int i = 0; i < BUILD_MAX; i++) {
    buildings[i] = 0;
  }
}

void City::interface_buildings
{
  cuss::interface i_buildings;
  Window w_buildings(0, 0, 80, 24);

  if (!i_buildings.load_from_file("cuss/buildings.cuss")) {
    return;
  }

  std::vector<std::string> building_names, building_count;

  for (int i = 0; i < BUILD_MAX; i++) {
    if (buildings[i] > 0) {
      building_names.push_back( Building_data[i]->name );
    }
  }
}
