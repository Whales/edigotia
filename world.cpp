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
  int temp_shift = 0;
  for (int x = 0; x < WORLD_MAP_SIZE; x++) {
    if (!one_in(3)) {
      temp_shift += rng(-1, 1);
    }
    if (temp_shift < -15) {
      temp_shift = -15;
    } else if (temp_shift > 15) {
      temp_shift = 15;
    }
    for (int y = 0; y < WORLD_MAP_SIZE; y++) {
      tiles[x][y] = MAP_OCEAN;
      altitude[x][y] = -1;
      rainfall[x][y] = -1;
      if (y < WORLD_MAP_SIZE / 2) {
        temperature[x][y] = (100 * (temp_shift + y))  / (WORLD_MAP_SIZE / 2);
      } else {
        int fy = WORLD_MAP_SIZE - 1 - y;
        temperature[x][y] = (100 * (temp_shift + fy)) / (WORLD_MAP_SIZE / 2);
      }
      //temperature[x][y] += rng(-1, 1);
      //temperature[x][y] += rng(-3, 3);
      continent_id[x][y] = -1;
      river[x][y] = false;
    }
  }

  continents.clear();

// Pick a bunch of points to base continents off of.
  for (int i = 0; i < 35; i++) {
    Point continent(rng(20, WORLD_MAP_SIZE - 21), rng(20, WORLD_MAP_SIZE - 21));
    continents.push_back(continent);
  }

// Build continent, and rivers there
  for (int i = 0; i < continents.size(); i++) {
    int height = rng(70, 125);
    int size = rng(3, 12);
    if (one_in(8)) { // Island
      if (one_in(2)) {  // Low-altitude island
        height = rng(20, 70);
        size = rng(2, 6);
      } else {  // Mountainous island
        height = rng(100, 150);
        size = rng(25, 35);
      }
    }
    add_continent(continents[i], height, size, i);
    Point river(continents[i].x + rng(-5, 5), continents[i].y + rng(-5, 5));
    add_river(river);
    int num = 3;
    while (one_in(num)) {
      river.x += rng(-4, 4);
      river.y += rng(-4, 4);
      add_river(river);
      num++;
    }
  }

// Set rainfall
  for (int y = 0; y < WORLD_MAP_SIZE; y++) {
    rainfall[0][y] = 35;
  }
  for (int x = 1; x < WORLD_MAP_SIZE; x++) {
    for (int y = 0; y < WORLD_MAP_SIZE; y++) {
      int past;
      int sources = 2;
      past = rainfall[x - 1][y] * 2;
      if (altitude[x][y] <= 0) {
        past += rng(0, 1);
      } else if (one_in(15)) {
        past -= rng(0, 1);
      }
      if (y > 0 && rainfall[x - 1][y - 1] > 0) {
        sources += 5;
        past += rainfall[x - 1][y - 1] * 5;
        if (rainfall[x - 1][y - 1] > rainfall[x - 1][y]) {
          sources += 2;
          past += rainfall[x - 1][y - 1] * 2;
        }
      }
      if (y < WORLD_MAP_SIZE - 1 && rainfall[x - 1][y + 1] > 0) {
        sources += 5;
        past += rainfall[x - 1][y + 1] * 5;
        if (rainfall[x - 1][y + 1] > rainfall[x - 1][y]) {
          sources += 2;
          past += rainfall[x - 1][y + 1] * 2;
        }
      }
      past /= sources;
      if (altitude[x][y] >= 85) {
        past = rng(0, past / 2);
      } else if (altitude[x][y] >= 60 && one_in(6)) {
        past -= rng(0, 4);
      } else if (altitude[x][y] < 85 - past && one_in(3)) {
        past = rng(past - 2, past + 4);
      }
      if (river[x][y]) {
        if (one_in(10)) {
          past += rng(10, 50);
        } else {
          past += rng(1, 15);
        }
      }
      if (past > temperature[x][y]) {
        past = rng(temperature[x][y], past);
      }
      rainfall[x][y] = past;
      if (rainfall[x][y] < 0) {
        rainfall[x][y] = 0;
      }
    }
  }

// "Flavor" continents
/*
  for (int i = 0; i < continents.size(); i++) {
    int flavor = rng(1, 20);
    int shift = rng(20, 40);

    for (int x = 0; x < WORLD_MAP_SIZE; x++) {
      for (int y = 0; y < WORLD_MAP_SIZE; y++) {

        int match = (continent_id[x][y] == i);
        for (int n = 0; !match && n < joined_continents[i].size(); n++) {
          if (joined_continents[i][n] == continent_id[x][y]) {
            match = true;
          }
        }
        if (match) {
          switch (flavor) {

            case 1: // High-altitude!
              altitude[x][y] += shift;
              if (one_in(10)) {
                altitude[x][y] += rng(10, 30);
              }
              break;

            case 2: // Low-altitude!
              altitude[x][y] -= shift;
              if (altitude[x][y] < 1) {
                altitude[x][y] = 1;
              }
              break;

            case 3: // Rainy!
              rainfall[x][y] += rng(30, 40);
              break;

            case 4: // Dry!
              rainfall[x][y] -= rng(40, 50);
              break;

            default:
              break;
          }
        }
      }
    }
  }
*/

// Finally, set terrain based on altitude & rainfall
  for (int x = 0; x < WORLD_MAP_SIZE; x++) {
    for (int y = 0; y < WORLD_MAP_SIZE; y++) {
      if (altitude[x][y] >= 80) {
        if (river[x][y]) {
          tiles[x][y] = MAP_CANYON;
        } else {
          tiles[x][y] = MAP_MOUNTAINOUS;
        }
      } else if (altitude[x][y] >= 55) {
        if (river[x][y]) {
          if (altitude[x][y] >= rng(60, 85) || temperature[x][y] <= 20) {
            tiles[x][y] = MAP_CANYON;
          } else {
            tiles[x][y] = MAP_BASIN;
          }
        } else if (rainfall[x][y] > altitude[x][y]) {
          tiles[x][y] = MAP_FOREST;
        } else {
          tiles[x][y] = MAP_FOOTHILLS;
        }
      } else if (altitude[x][y] <= 0) {
        tiles[x][y] = MAP_OCEAN;
      } else {
        if (temperature[x][y] <= 20) {
          tiles[x][y] = MAP_TUNDRA;
        } else if (rainfall[x][y] >= 62) {
          tiles[x][y] = MAP_SWAMP;
        } else if (river[x][y] && temperature[x][y] > 20) {
          tiles[x][y] = MAP_BASIN;
        } else if (rainfall[x][y] >= 45) {
          tiles[x][y] = MAP_FOREST;
        } else if (rainfall[x][y] >= 30) {
          tiles[x][y] = MAP_PLAINS;
        } else if (rainfall[x][y] >= 12) {
          tiles[x][y] = MAP_WASTELAND;
        } else if (temperature[x][y] <= 32) {
          tiles[x][y] = MAP_WASTELAND;
        } else {
          tiles[x][y] = MAP_DESERT;
        }
      }
    }
  }
}

void World_map::add_continent(Point origin, int height, int step, int id)
{
  std::vector<Point> active;

  active.push_back(origin);

  altitude[origin.x][origin.y] = height;
  continent_id[origin.x][origin.y] = id;

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
          int dist_to_edge = (x < y ? x : y);
          if (WORLD_MAP_SIZE - 1 - x < dist_to_edge) {
            dist_to_edge = WORLD_MAP_SIZE - 1 - x;
          }
          if (WORLD_MAP_SIZE - 1 - y < dist_to_edge) {
            dist_to_edge = WORLD_MAP_SIZE - 1 - y;
          }
          if (dist_to_edge < 10) {
            altitude[x][y] -= rng((10 - dist_to_edge), 15*(10 - dist_to_edge));
          } else if (one_in(30)) {
            altitude[x][y] -= rng(0, 100);
          } else if (!one_in(10)) {
            altitude[x][y] -= rng(0, step);
          }
          if (altitude[x][y] > 0) {
            new_points.push_back(Point(x, y));
            if (continent_id[x][y] != -1) {
// Joined continents!
              int other_id = continent_id[x][y];
              if (joined_continents.count(id) == 0) {
                std::vector<int> tmp;
                tmp.push_back(other_id);
                joined_continents[id] = tmp;
              } else {
// Check if we've already noted this
                bool found = false;
                for (int i = 0; !found && i<joined_continents[id].size(); i++) {
                  if (joined_continents[id][i] == other_id) {
                    found = true;
                  }
                }
                if (!found) {
                  joined_continents[id].push_back(other_id);
                  if (joined_continents.count(other_id) == 0) {
                    std::vector<int> tmp;
                    tmp.push_back(id);
                    joined_continents[other_id] = tmp;
                  } else {
                    joined_continents[other_id].push_back(id);
                  }
                }
              }
            } else {
              continent_id[x][y] = id;
            }
          } else {
            altitude[x][y] = 0; // "False" ocean
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

void World_map::draw(Window* w_map)
{
  bool owns_window = false;
  int xdim, ydim;
  if (w_map == NULL) {
    owns_window = true;
    get_screen_dims(xdim, ydim);
    w_map = new Window(0, 0, xdim, ydim);
  } else {
    xdim = w_map->sizex();
    ydim = w_map->sizey();
  }

  int cur_cont = 0;
  Point pos = continents[0];
  pos.x -= (xdim / 2);
  pos.y -= (ydim / 2);

  while (true) {
    for (int x = pos.x; x < pos.x + xdim; x++) {
      for (int y = pos.y; y < pos.y + ydim; y++) {
        if (x > 0 && x < WORLD_MAP_SIZE && y > 0 && y < WORLD_MAP_SIZE) {
          Map_type type = tiles[x][y];
          Map_type_datum* data = Map_type_data[type];
          w_map->putglyph(x - pos.x, y - pos.y, data->symbol);
        } else {
          w_map->putglyph(x - pos.x, y - pos.y, glyph('x', c_white, c_black));
        }
      }
    }
    w_map->refresh();
    long ch = getch();
    switch (ch) {
      case '>':
        if (cur_cont >= continents.size() - 1) {
          cur_cont = 0;
        } else {
          cur_cont++;
        }
        pos.x = continents[cur_cont].x - (xdim / 2);
        pos.y = continents[cur_cont].y - (ydim / 2);
        break;
      case '<':
        if (cur_cont <= 0) {
          cur_cont = continents.size() - 1;
        } else {
          cur_cont--;
        }
        pos.x = continents[cur_cont].x - (xdim / 2);
        pos.y = continents[cur_cont].y - (ydim / 2);
        break;
      case 'Y':
        pos.x -= 10;
        pos.y -= 10;
        break;
      case 'K':
        pos.y -= 10;
        break;
      case 'U':
        pos.x += 10;
        pos.y -= 10;
        break;
      case 'H':
        pos.x -= 10;
        break;
      case 'L':
        pos.x += 10;
        break;
      case 'B':
        pos.x -= 10;
        pos.y += 10;
        break;
      case 'J':
        pos.y += 10;
        break;
      case 'N':
        pos.x += 10;
        pos.y += 10;
        break;
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
        if (owns_window) {
          delete w_map;
        }
        return;
    }
  }
}
