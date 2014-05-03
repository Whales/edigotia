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
      altitude[x][y] = -1;
      rainfall[x][y] = -1;
      river[x][y] = false;
    }
  }

// Pick a bunch of points to base continents off of.
  std::vector<Point> continents;
  for (int i = 0; i < 35; i++) {
    Point continent(rng(20, WORLD_MAP_SIZE - 21), rng(20, WORLD_MAP_SIZE - 21));
    continents.push_back(continent);
  }

// Build continent, and rivers there
  for (int i = 0; i < continents.size(); i++) {
    add_continent(continents[i], rng(3, 12));
    Point river(continents[i].x + rng(-5, 5), continents[i].y + rng(-5, 5));
    add_river(river);
    while (one_in(3)) {
      river.x += rng(-2, 2);
      river.y += rng(-2, 2);
      add_river(river);
    }
  }

// Set rainfall
  int x = 0;
  for (int y = 0; y < WORLD_MAP_SIZE; y++) {
    if (altitude[x][y] < 85) {
      rainfall[x][y] = 85 - altitude[x][y];
    }
  }
  for (int x = 1; x < WORLD_MAP_SIZE; x++) {
    for (int y = 0; y < WORLD_MAP_SIZE; y++) {
      int past;
      if (altitude[x][y] <= 0) {
        past = rng(10, 60);
      } else {
        past = rainfall[x - 1][y];
      }
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
      if (altitude[x][y] >= 85) {
        past = 0;
      } else if (altitude[x][y] >= 60 && one_in(6)) {
        past -= rng(0, 4);
      } else if (altitude[x][y] < 85 - past && one_in(4)) {
        past = rng(past - 2, past + 4);
      }
      if (river[x][y]) {
        past += rng(1, 15);
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
      if (altitude[x][y] >= 85) {
        if (river[x][y]) {
          tiles[x][y] = MAP_CANYON;
        } else {
          tiles[x][y] = MAP_MOUNTAINOUS;
        }
      } else if (river[x][y]) {
        tiles[x][y] = MAP_BASIN;
      } else if (altitude[x][y] >= 60) {
        if (rainfall[x][y] > altitude[x][y]) {
          tiles[x][y] = MAP_FOREST;
        } else {
          tiles[x][y] = MAP_FOOTHILLS;
        }
      } else if (altitude[x][y] <= 0) {
        tiles[x][y] = MAP_OCEAN;
      } else {
        if (rainfall[x][y] >= 55) {
          tiles[x][y] = MAP_SWAMP;
        } else if (rainfall[x][y] >= 45) {
          tiles[x][y] = MAP_FOREST;
        } else if (rainfall[x][y] >= 30) {
          tiles[x][y] = MAP_PLAINS;
        } else if (rainfall[x][y] >= 12) {
          tiles[x][y] = MAP_WASTELAND;
        } else {
          tiles[x][y] = MAP_DESERT;
        }
      }
    }
  }
}

void World_map::add_continent(Point origin, int step)
{
  int size = 0;
  std::vector<Point> active;

  active.push_back(origin);

  altitude[origin.x][origin.y] = rng(70, 120);

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
// Check if we made the continent larger
          int dist = rl_dist(origin, p);
          if (dist > size) {
            size = dist;
          }
          altitude[x][y] = altitude[p.x][p.y];
          //debugmsg("%d", altitude[x][y]);
          if (one_in(30)) {
            altitude[x][y] -= rng(0, 100);
          } else if (!one_in(10)) {
            altitude[x][y] -= rng(0, step);
          }
          if (altitude[x][y] > 0) {
            new_points.push_back(Point(x, y));
          } else {
            altitude[x][y] = 0; // "False" ocean
          }
        }
      }
      active.erase(active.begin() + index);
    }
    active = new_points;
  }

// Get rid of any ocean inside our range
/*
  int ocean_replacement = rng(1, 100);
  int start_x = origin.x - size, end_x = origin.x + size,
      start_y = origin.y - size, end_y = origin.y + size;
  if (start_x < 0) {
    start_x = 0;
  }
  if (end_x > WORLD_MAP_SIZE - 1) {
    end_x = WORLD_MAP_SIZE - 1;
  }
  if (start_y < 0) {
    start_y = 0;
  }
  if (end_y > WORLD_MAP_SIZE - 1) {
    end_y = WORLD_MAP_SIZE - 1;
  }
  for (int x = start_x; x <= end_x; x++) {
    for (int y = start_y; y <= end_y; y++) {
      if (altitude[x][y] <= 0) {
        altitude[x][y] = ocean_replacement;
      }
    }
  }
*/
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
// Check adjacent tiles for possible next tiles
    int lowest = 500;
    std::vector<Point> candidates;
    for (int x = origin.x - 1; x <= origin.x + 1; x++) {
      for (int y = origin.y - 1; y <= origin.y + 1; y++) {
        if (x == origin.x && y == origin.y) {
          y++;  // Skip the center tile
        }
        if (x >= 0 && x < WORLD_MAP_SIZE && y >= 0 && y <= WORLD_MAP_SIZE) {
          int alt = altitude[x][y];
// Check for adjacent rivers
          int adj_rivers = 0;
          for (int rx = x - 1; rx <= x + 1; rx++) {
            for (int ry = y - 1; ry <= y + 1; ry++) {
              if (rx >= 0 && rx < WORLD_MAP_SIZE &&
                  ry >= 0 && ry <= WORLD_MAP_SIZE && river[rx][ry]) {
                adj_rivers++;
              }
            }
          }
          alt += 40 * adj_rivers;
          if (alt < lowest && !river[x][y]) {
            candidates.clear();
            //lowest = altitude[x][y]; // Replace with lowest = alt; ?
            lowest = alt;
            candidates.push_back( Point(x, y) );
          } else if (alt == lowest && !river[x][y]) {
            candidates.push_back( Point(x, y) );
          }
        } else {
          done = true;  // Went off the map
        }
      }
    }

// Check if done
    if (candidates.empty()) {
      done = true;
    }
    for (int i = 0; !done && i < candidates.size(); i++) {
      Point p = candidates[i];
      if (altitude[p.x][p.y] <= -1 || river[p.x][p.y]) {
        done = true;
      }
    }

    if (!done) {
      int index = rng(0, candidates.size() - 1);
      origin = candidates[index];
      candidates.erase(candidates.begin() + index);
    }
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
