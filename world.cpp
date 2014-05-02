#include "world.h"
#include "rng.h"
#include "geometry.h"
#include <vector>

World_map::World_map()
{
}

World_map::~World_map()
{
}

void World_map::generate()
{
  for (int x = 0; x < WORLD_MAP_SIZE; x++) {
    for (int y = 0; y < WORLD_MAP_SIZE; y++) {
      tiles[x][y] = MAP_OCEAN;
      altitude[x][y] = 0;
      rainfall[x][y] = -1;
      river[x][y] = false;
    }
  }

// Pick a bunch of points to base continents off of.
  std::vector<Point> continents;
  for (int i = 0; i < 5; i++) {
    Point continent(rng(20, WORLD_MAP_SIZE - 21), rng(20, WORLD_MAP_SIZE - 21));
    continents.push_back(continent);
  }

// Build continent, and rivers there
  for (int i = 0; i < continents.size(); i++) {
    add_continent(continents[i]);
    Point river(continents[i].x + rng(-15, 15), continents[i].y + rng(-15, 15));
    add_river(river);
  }

// Set rainfall
  int x = 0;
  for (int y = 0; y < WORLD_MAP_SIZE; y++) {
    if (altitude[x][y] < 70) {
      rainfall[x][y] = 70 - altitude[x][y];
    }
  }
  for (int x = 1; x < WORLD_MAP_SIZE; x++) {
    for (int y = 0; y < WORLD_MAP_SIZE; y++) {
      int past = rainfall[x - 1][y];
      if (past <= 0) {
        int newpast = 0;
        int sources = 0;
        if (y > 0 && rainfall[x - 1][y - 1] > 0) {
          sources++;
          newpast += rainfall[x - 1][y - 1];
        }
        if (y < WORLD_MAP_SIZE - 1 && rainfall[x - 1][y + 1] > 0) {
          sources++;
          newpast += rainfall[x - 1][y + 1];
        }
        if (sources > 0) {
          newpast /= sources;
        }
        past = newpast;
      }
      if (altitude[x][y] >= 70) {
        past = 0;
      } else if (altitude[x][y] < 70 - past) {
        past++;
      }
      rainfall[x][y] = past;
      if (past > 0) {
        rainfall[x][y] += rng(-3, 3);
      }
      if (rainfall[x][y] < 0) {
        rainfall[x][y] = 0;
      }
    }
  }

// Finally, set terrain based on altitude & rainfall
  for (int x = 0; x < WORLD_MAP_SIZE; x++) {
    for (int y = 0; y < WORLD_MAP_SIZE; y++) {
      if (altitude[x][y] >= 70) {
        if (river) {
          tiles[x][y] = MAP_CANYON;
        } else {
          tiles[x][y] = MAP_MOUNTAINOUS;
        }
      } else if (river) {
        tiles[x][y] = MAP_BASIN;
      } else if (altitude[x][y] >= 50) {
        if (rainfall[x][y] > altitude[x][y]) {
          tiles[x][y] = MAP_FOREST;
        } else {
          tiles[x][y] = MAP_FOOTHILLS;
        }
      } else if (altitude[x][y] <= 0) {
        tiles[x][y] = MAP_OCEAN;
      } else {
        if (rainfall[x][y] >= 50) {
          tiles[x][y] = MAP_SWAMP;
        } else if (rainfall[x][y] >= 30) {
          tiles[x][y] = MAP_FOREST;
        } else if (rainfall[x][y] >= 20) {
          tiles[x][y] = MAP_PLAINS;
        } else if (rainfall[x][y] >= 10) {
          tiles[x][y] = MAP_ROCKY;
        } else {
          tiles[x][y] = MAP_DESERT;
        }
      }
    }
  }
}

void World_map::add_continent(Point origin)
{
  std::vector<Point> active;

  active.push_back(origin);

  altitude[origin.x][origin.y] = rng(80, 100);

  while (!active.empty()) {
    std::vector<Point> new_points;
    while (!active.empty()) {
      int index = rng(0, active.size() - 1);
      Point p = active[index];

      for (int i = 0; i < 4; i++) {
        int x, y;
        switch (i) {
          case 0: x = p.x - 1; y = p.y;     break;
          case 1: x = p.x + 1; y = p.y;     break;
          case 2: x = p.x;     y = p.y - 1; break;
          case 3: x = p.x;     y = p.y + 1; break;
        }
        if (x > 0 && x < WORLD_MAP_SIZE && y > 0 && y < WORLD_MAP_SIZE &&
            altitude[x][y] == 0) {
          new_points.push_back(Point(x, y));
          altitude[x][y] = altitude[p.x][p.y];
          if (one_in(30)) {
            altitude[x][y] -= rng(0, 100);
          } else if (!one_in(10)) {
            altitude[x][y] -= rng(0, 5);
          }
        }
      }
      active.erase(active.begin() + index);
    }
    active = new_points;
  }
}

void World_map::add_river(Point origin)
{
  river[origin.x][origin.y] = true;
  int lowest = 500;
  std::vector<Point> candidates;
  for (int x = origin.x - 1; x <= origin.x + 1; x++) {
    for (int y = origin.y - 1; y <= origin.y + 1; y++) {
      if (altitude[x][y] < lowest) {
        candidates.clear();
        lowest = altitude[x][y];
        candidates.push_back( Point(x, y) );
      } else if (altitude[x][y] == lowest) {
        candidates.push_back( Point(x, y) );
      }
    }
  }

  while (!candidates.empty()) {
    int index = rng(0, candidates.size() - 1);
    Point p = candidates[index];
    if (!river[p.x][p.y]) {
      add_river(p);
    }
    candidates.erase(candidates.begin() + index);
  }
}
