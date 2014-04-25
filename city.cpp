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
  std::vector<Building_type> building_types; // To key name list to types

  for (int i = 0; i < BUILD_MAX; i++) {
    if (buildings[i] > 0) {
      building_names.push_back( Building_data[i]->name );
      building_count.push_back( itos(buildings[i])     );
      building_types.push_back( Building_type(i)       );
    }
  }

  i_buildings.set_data("list_buildings", building_names);
  i_buildings.set_data("list_count",     building_count);
  i_buildings.select("list_buildings");

  bool done = false;
  while (!done) {
    i_buildings.draw(&w_buildings);
    w_buildings.refresh();

    int index = i_buildings.get_int("list_buildings");
    Building_type type = building_types[index];

    i_buildings.set_data("num_maintenance", Building_data[type]->upkeep);

    long ch = input();
    if (ch == 'q' || ch == 'Q') {
      done = true;
    } else {
      i_buildings.handle_keypress(ch);
    }
  }
}
