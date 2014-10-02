#include "world.h"
#include "rng.h"
#include "geometry.h"
#include "window.h"
#include <sstream>
#include <vector>
#include <math.h>

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
  for (int i = 0; i < 45; i++) {
    Point continent(rng(20, WORLD_MAP_SIZE - 21), rng(20, WORLD_MAP_SIZE - 21));
    continents.push_back(continent);
  }

// Build continent, and rivers there
  for (int i = 0; i < continents.size(); i++) {
    int height = rng(70, 125);
    int size = rng(3, 8);
    if (one_in(8)) { // Island
      if (one_in(2)) {  // Low-altitude island
        height = rng(20, 50);
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
        if (one_in(4)) {
          past += rng(1, 2);
        }
      } else if (one_in(15)) {
        past -= rng(0, 1);
      }
      if (y > 0 && rainfall[x - 1][y - 1] > 0) {
        sources += 5;
        past += rainfall[x - 1][y - 1] * 5;
        if (rainfall[x - 1][y - 1] > rainfall[x - 1][y]) {
          sources += 1;
          past += rainfall[x - 1][y - 1];
        }
      }
      if (y < WORLD_MAP_SIZE - 1 && rainfall[x - 1][y + 1] > 0) {
        sources += 5;
        past += rainfall[x - 1][y + 1] * 5;
        if (rainfall[x - 1][y + 1] > rainfall[x - 1][y]) {
          sources += 1;
          past += rainfall[x - 1][y + 1];
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
        if (altitude[x][y] == 0 && temperature[x][y] <= 18) {
          tiles[x][y] = MAP_ICECAP;
        } else {
          tiles[x][y] = MAP_OCEAN;
        }
      } else {
        if (temperature[x][y] <= 20) {
          tiles[x][y] = MAP_TUNDRA;
        } else if (rainfall[x][y] >= 55) {
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

// ... aaaand set up crops/minerals.
  for (int x = 0; x < WORLD_MAP_SIZE; x++) {
    for (int y = 0; y < WORLD_MAP_SIZE; y++) {
      crops   [x][y] = 0;
      minerals[x][y] = 0;
    }
  }
  for (int i = 1; i < CROP_MAX; i++) {
    Crop crop = Crop(i);
    Crop_datum* crop_dat = Crop_data[crop];
    int min_radius = crop_dat->percentage / 5;
    if (min_radius < 2) {
      min_radius = 2;
    }
    int max_radius = crop_dat->percentage / 2;
    int avg_radius = (min_radius + max_radius) / 2;
    int avg_size   = avg_radius * avg_radius;
// Calculate the total number of blobs of the given size that'd fit in the world
    int total_blobs = WORLD_MAP_SIZE * WORLD_MAP_SIZE;
    total_blobs /= avg_size;
    if (crop_dat->percentage < 20) {
      total_blobs *= .6;
    } else if (crop_dat->percentage > 90) {
      total_blobs *= 1.2;
    }
// Place an appropriate percentage of the total blobs
    int num_blobs = total_blobs * crop_dat->percentage;
    num_blobs /= 100;
    //debugmsg("%s blobs: %d", Crop_data[crop]->name.c_str(), num_blobs);
// Now place the blobs.
    int checkpoint = num_blobs / 20;  // 5% checkpoints
    if (crop_dat->percentage >= 90) {
      max_radius += 8;
    }
    for (int n = 0; n < num_blobs; n++) {
      if (n % checkpoint == 1) {
        popup_nowait("\
Generating World...\n\
Crops: %d / %d  \n\
Placing %d blobs [%d%%%%%%%%]",
                     i, CROP_MAX - 1, num_blobs, (100 * n) / num_blobs);
      }
      int radius = rng(min_radius, max_radius);
      Point p( rng(0, WORLD_MAP_SIZE - 1), rng(0, WORLD_MAP_SIZE - 1) );
      add_crop(p, crop, radius);
    }
  }

// Exact same thing, but for minerals.
  for (int i = 1; i < MINERAL_MAX; i++) {
    Mineral mineral = Mineral(i);
    Mineral_datum* mineral_dat = Mineral_data[mineral];
    int min_radius = mineral_dat->percentage / 5;
    if (min_radius < 2) {
      min_radius = 2;
    }
    int max_radius = mineral_dat->percentage / 2;
    int avg_radius = (min_radius + max_radius) / 2;
    int avg_size   = avg_radius * avg_radius;
// Calculate the total number of blobs of the given size that'd fit in the world
    int total_blobs = WORLD_MAP_SIZE * WORLD_MAP_SIZE;
    total_blobs /= avg_size * 1.2;
    if (mineral_dat->percentage < 20) {
      total_blobs *= .6;
    } else if (mineral_dat->percentage > 90) {
      total_blobs *= 1.2;
    }
// Place an appropriate percentage of the total blobs
    int num_blobs = total_blobs * mineral_dat->percentage;
    num_blobs /= 100;
    //debugmsg("%s blobs: %d", Mineral_data[mineral]->name.c_str(), num_blobs);
    if (mineral_dat->percentage >= 90) {
      max_radius += 8;
    }
// Now place the blobs.
    int checkpoint = num_blobs / 20;  // 5% checkpoints
    for (int n = 0; n < num_blobs; n++) {
      if (n % checkpoint == 1) {
        popup_nowait("\
Generating World...\n\
Minerals: %d / %d  \n\
Placing %d blobs [%d%%%%%%%%]",
                     i, MINERAL_MAX - 1, num_blobs, (100 * n) / num_blobs);
      }
      int radius = rng(min_radius, max_radius);
      Point p( rng(0, WORLD_MAP_SIZE - 1), rng(0, WORLD_MAP_SIZE - 1) );
      add_mineral(p, mineral, radius);
    }
  }

// Fix clustered rivers
/*
  std::vector<Point> swamps;
  for (int x = 0; x < WORLD_MAP_SIZE - 2; x++) {
    for (int y = 0; y < WORLD_MAP_SIZE - 2; y++) {
      int count = 0;
      for (int rx = x; rx <= x + 10; rx++) {
        for (int ry = y; ry <= y + 10; ry++) {
          if (tiles[rx][ry] == MAP_BASIN) {
            count++;
          }
        }
      }
      if (count >= 35) {
        for (int rx = x; rx <= x + 10; rx++) {
          for (int ry = y; ry <= y + 10; ry++) {
            if (tiles[rx][ry] == MAP_BASIN || one_in(6)) {
              swamps.push_back( Point(rx, ry) );
            }
          }
        }
      }
    }
  }
  for (int i = 0; i < swamps.size(); i++) {
    tiles[swamps[i].x][swamps[i].y] = MAP_SWAMP;
  }
*/
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
          if (dist_to_edge < 20) {
            altitude[x][y] -= rng(0, 2 * (20 - dist_to_edge));
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

void World_map::add_crop(Point origin, Crop crop, int radius)
{
  //crops.push_back( Crop_area(crop, origin, radius) );
  add_resource(origin, crop, MINERAL_NULL, radius);
}

void World_map::add_mineral(Point origin, Mineral mineral, int radius)
{
  //minerals.push_back( Mineral_area(mineral, origin, radius) );
  add_resource(origin, CROP_NULL, mineral, radius);
}

void World_map::add_resource(Point origin, Crop crop, Mineral mineral,
                             int radius)
{
  if (crop == CROP_NULL && mineral == MINERAL_NULL) {
    debugmsg("World_map::add_resource() called with crop = CROP_NULL and \
mineral = MINERAL_NULL!");
    return;
  } else if (crop != CROP_NULL && mineral != MINERAL_NULL) {
    debugmsg("World_map::add_resource() called with crop AND mineral!");
    return;
  }
  if (origin.x < 0               || origin.y < 0 ||
      origin.x >= WORLD_MAP_SIZE || origin.y >= WORLD_MAP_SIZE) {
    debugmsg("World_map::add_resource() called with origin %s.",
             origin.str().c_str());
    return;
  }

// Basically, we're doing the same thing as add_continent(); set the "level" at
// our origin to a high amount, slope downwards in all directions.
  std::vector<Point> active;
  std::vector<Point> placement;
  active.push_back(origin);
  placement.push_back(origin);

  int level[WORLD_MAP_SIZE][WORLD_MAP_SIZE];
  for (int x = 0; x < WORLD_MAP_SIZE; x++) {
    for (int y = 0; y < WORLD_MAP_SIZE; y++) {
      level[x][y] = 0;
    }
  }

  level[origin.x][origin.y] = 100;
  int step = 100 / radius;

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
            level[x][y] <= 0) {
          level[x][y] = level[p.x][p.y];
          if (one_in(30)) {
            level[x][y] -= rng(0, 100);
          } else if (!one_in(10)) {
            level[x][y] -= rng(step, step * 2);
          }
          if (level[x][y] > 0) {
            new_points.push_back(Point(x, y));
            placement.push_back (Point(x, y));
          } else {
            level[x][y] = 0;
          }
        }
      }
      active.erase(active.begin() + index);
    }
    active = new_points;
  }

//debugmsg("Placed %d tiles.", placement.size());
// Now place our resource on all points in placement
  for (int i = 0; i < placement.size(); i++) {
    int x = placement[i].x, y = placement[i].y;
    if (tiles[x][y] != MAP_NULL && tiles[x][y] != MAP_OCEAN) {
      if (crop != CROP_NULL) {
  // Check if we already exist
        bool found = crops[x][y] & int(pow(2, crop));
        if (!found) {
          crops[x][y] |= int(pow(2, crop));
        }
      } else {  // Placing mineral, not crop.
  // Check if we already exist
        bool found = minerals[x][y] & int(pow(2, mineral));
        if (!found) {
          minerals[x][y] |= int(pow(2, mineral));
        }
      }
    }
  }
}

Point World_map::draw(Window* w_map)
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
          glyph gl = data->symbol;
          if (x == pos.x + xdim / 2 && y == pos.y + ydim / 2) {
            gl = gl.hilite(c_blue);
          }
          w_map->putglyph(x - pos.x, y - pos.y, gl);
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

      case '?': {
        std::stringstream resource_ss;
        std::vector<Crop> crops_here = crops_at(pos);
        std::vector<Mineral> minerals_here = minerals_at(pos);
        resource_ss << "Crops: ";
        for (int i = 0; i < crops_here.size(); i++) {
          resource_ss << Crop_data[crops_here[i]]->name << " ";
        }
        resource_ss << std::endl << "Minerals: ";
        for (int i = 0; i < minerals_here.size(); i++) {
          resource_ss << Mineral_data[minerals_here[i]]->name << " ";
        }
        debugmsg( resource_ss.str().c_str() );
      } break;
      case KEY_ESC:
      case '\n':
        if (owns_window) {
          delete w_map;
        }
        if (ch == '\n') {
          pos.x += (xdim / 2);
          pos.y += (ydim / 2);
          return pos;
        }
        return Point(-1, -1);
    }
  }
}

int World_map::land_count()
{
  int ret = 0;
  for (int x = 0; x < WORLD_MAP_SIZE; x++) {
    for (int y = 0; y < WORLD_MAP_SIZE; y++) {
      if (tiles[x][y] != MAP_NULL && tiles[x][y] != MAP_OCEAN) {
        ret++;
      }
    }
  }
  return ret;
}

// crop defaults to CROP_NULL
int World_map::crop_count(Crop crop)
{
  int ret = 0;
  for (int x = 0; x < WORLD_MAP_SIZE; x++) {
    for (int y = 0; y < WORLD_MAP_SIZE; y++) {
      if (has_crop(crop, x, y)) {
        ret++;
      }
    }
  }
  return ret;
}

// mineral defaults to MINERAL_NULL
int World_map::mineral_count(Mineral mineral)
{
  int ret = 0;
  for (int x = 0; x < WORLD_MAP_SIZE; x++) {
    for (int y = 0; y < WORLD_MAP_SIZE; y++) {
      if (has_mineral(mineral, x, y)) {
        ret++;
      }
    }
  }
  return ret;
}

Map_type World_map::get_map_type(Point p)
{
  return get_map_type(p.x, p.y);
}

Map_type World_map::get_map_type(int x, int y)
{
  if (x < 0 || x >= WORLD_MAP_SIZE || y < 0 || y >= WORLD_MAP_SIZE) {
    return MAP_NULL;
  }
  return tiles[x][y];
}
  
Direction World_map::coast_from(Point p)
{
  return coast_from(p.x, p.y);
}

Direction World_map::coast_from(int x, int y)
{
  if (x < 0 || y < 0 || x >= WORLD_MAP_SIZE || y >= WORLD_MAP_SIZE) {
    return DIR_NULL;
  }
  std::vector<Direction> candidates;
  if (x > 0 && tiles[x - 1][y] == MAP_OCEAN) {
    candidates.push_back(DIR_WEST);
  }
  if (x < WORLD_MAP_SIZE - 1 && tiles[x + 1][y] == MAP_OCEAN) {
    candidates.push_back(DIR_EAST);
  }
  if (y > 0 && tiles[x][y - 1] == MAP_OCEAN) {
    candidates.push_back(DIR_NORTH);
  }
  if (y < WORLD_MAP_SIZE - 1 && tiles[x][y + 1] == MAP_OCEAN) {
    candidates.push_back(DIR_SOUTH);
  }

  if (candidates.empty()) {
    return DIR_NULL;
  }
  return candidates[ rng(0, candidates.size() - 1) ];
}

std::vector<Crop> World_map::crops_at(Point p)
{
  return crops_at( p.x, p.y );
}

std::vector<Mineral> World_map::minerals_at(Point p)
{
  return minerals_at( p.x, p.y );
}

std::vector<Crop> World_map::crops_at(int x, int y)
{
  std::vector<Crop> ret;
  for (int i = 1; i < CROP_MAX; i++) {
    if (has_crop( Crop(i), x, y )) {
      ret.push_back( Crop(i) );
    }
  }
  return ret;
}

std::vector<Mineral> World_map::minerals_at(int x, int y)
{
  std::vector<Mineral> ret;
  for (int i = 1; i < MINERAL_MAX; i++) {
    if (has_mineral( Mineral(i), x, y )) {
      ret.push_back( Mineral(i) );
    }
  }
  return ret;
}

bool World_map::has_crop(Crop crop, Point p)
{
  return has_crop(crop, p.x, p.y);
}

bool World_map::has_mineral(Mineral mineral, Point p)
{
  return has_mineral(mineral, p.x, p.y);
}

bool World_map::has_crop(Crop crop, int x, int y)
{
  if (x < 0 || x >= WORLD_MAP_SIZE ||
      y < 0 || y >= WORLD_MAP_SIZE) {
    return false;
  }
  if (crop == CROP_NULL && crops[x][y] > 1) {
    return true;
  }
  return (crops[x][y] & int(pow(2, crop)));
}

bool World_map::has_mineral(Mineral mineral, int x, int y)
{
  if (x < 0 || x >= WORLD_MAP_SIZE ||
      y < 0 || y >= WORLD_MAP_SIZE) {
    return false;
  }
  if (mineral == MINERAL_NULL && minerals[x][y] > 1) {
    return true;
  }
  return (minerals[x][y] & int(pow(2, mineral)));
}
