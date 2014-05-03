#include "world.h"
#include "rng.h"
#include "geometry.h"
#include "window.h"
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
  for (int i = 0; i < 50; i++) {
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
      int sources = 1;
      if (y > 0 && rainfall[x - 1][y - 1] > 0) {
        sources++;
        past += rainfall[x - 1][y - 1];
      }
      if (y < WORLD_MAP_SIZE - 1 && rainfall[x - 1][y + 1] > 0) {
        sources++;
        past += rainfall[x - 1][y + 1];
      }
      past /= sources;
      if (altitude[x][y] <= 0) {
        past -= rng(0, 5);
      } else if (altitude[x][y] >= 70) {
        past = 0;
      } else if (altitude[x][y] < 70 - past) {
        past = rng(past - 2, past + 4);
      }
      rainfall[x][y] = past;
      if (rainfall[x][y] < 0) {
        rainfall[x][y] = 0;
      }
    }
  }

// Finally, set terrain based on altitude & rainfall
  for (int x = 0; x < WORLD_MAP_SIZE; x++) {
    for (int y = 0; y < WORLD_MAP_SIZE; y++) {
      if (altitude[x][y] >= 70) {
        if (river[x][y]) {
          tiles[x][y] = MAP_CANYON;
        } else {
          tiles[x][y] = MAP_MOUNTAINOUS;
        }
      } else if (river[x][y]) {
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
        if (rainfall[x][y] >= 65) {
          tiles[x][y] = MAP_SWAMP;
        } else if (rainfall[x][y] >= 45) {
          tiles[x][y] = MAP_FOREST;
        } else if (rainfall[x][y] >= 30) {
          tiles[x][y] = MAP_PLAINS;
        } else if (rainfall[x][y] >= 10) {
          tiles[x][y] = MAP_WASTELAND;
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

  altitude[origin.x][origin.y] = 100;

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
            altitude[x][y] <= 0) {
          altitude[x][y] = altitude[p.x][p.y];
          //debugmsg("%d", altitude[x][y]);
          if (one_in(30)) {
            altitude[x][y] -= rng(0, 100);
          } else if (!one_in(10)) {
            altitude[x][y] -= rng(0, 5);
          }
          if (altitude[x][y] > 0) {
            new_points.push_back(Point(x, y));
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
  if (origin.x < 0 || origin.x >= WORLD_MAP_SIZE ||
      origin.y < 0 || origin.y >= WORLD_MAP_SIZE) {
    return;
  }

  bool done = false;
  while (!done) {
    river[origin.x][origin.y] = true;
    int lowest = 500;
    std::vector<Point> candidates;
    for (int x = origin.x - 1; x <= origin.x + 1; x++) {
      for (int y = origin.y - 1; y <= origin.y + 1; y++) {
        if (x >= 0 && x < WORLD_MAP_SIZE && y >= 0 && y <= WORLD_MAP_SIZE) {
          if (altitude[x][y] < lowest) {
            candidates.clear();
            lowest = altitude[x][y];
            candidates.push_back( Point(x, y) );
          } else if (altitude[x][y] == lowest && !river[x][y]) {
            candidates.push_back( Point(x, y) );
          }
        } else {
          done = true;  // Went off the map
        }
      }
    }

// Check if done
    for (int i = 0; !done && i < candidates.size(); i++) {
      Point p = candidates[i];
      if (altitude[p.x][p.y] <= 0 || river[p.x][p.y]) {
        done = true;
      }
    }

    int index = rng(0, candidates.size() - 1);
    origin = candidates[index];
    candidates.erase(candidates.begin() + index);
/*
    if (!candidates.empty()) {
      add_river( candidates[ rng(0, candidates.size() - 1) ] );
    }
*/
  }

}

void World_map::draw()
{
  Window w_map(0, 0, 80, 24);

  Point pos(WORLD_MAP_SIZE / 2, WORLD_MAP_SIZE / 2);

  while (true) {
    for (int x = pos.x; x < pos.x + 80; x++) {
      for (int y = pos.y; y < pos.y + 24; y++) {
        if (x > 0 && x < WORLD_MAP_SIZE && y > 0 && y < WORLD_MAP_SIZE) {
          Map_type type = tiles[x][y];
          Map_type_datum* data = Map_type_data[type];
          w_map.putglyph(x - pos.x, y - pos.y, data->symbol);
        }
      }
    }
    w_map.refresh();
    long ch = getch();
    switch (ch) {
      case 'y':
      case '7':
        pos.x--;
        pos.y--;
        break;
      case 'k':
      case '8':
      case KEY_UP:
        pos.y--;
        break;
      case 'u':
      case '9':
        pos.x++;
        pos.y--;
        break;
      case 'h':
      case '4':
      case KEY_LEFT:
        pos.x--;
        break;
      case 'l':
      case '6':
      case KEY_RIGHT:
        pos.x++;
        break;
      case 'b':
      case '1':
        pos.x--;
        pos.y++;
        break;
      case 'j':
      case '2':
      case KEY_DOWN:
        pos.y++;
        break;
      case 'n':
      case '3':
        pos.x++;
        pos.y++;
        break;
      case KEY_ESC:
      case '\n':
        return;
    }
  }
}
