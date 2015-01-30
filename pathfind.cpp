#include "pathfind.h"
#include "rng.h"
#include "window.h"
#include <math.h>
#include <algorithm>
#include <sstream>    // For Generic_map::save_data()

Path::Path()
{
  total_cost = 0;
}

Path::~Path()
{
}

std::vector<Point> Path::get_points()
{
  return path;
}

int Path::get_cost()
{
  return total_cost;
}

Point Path::step(int n)
{
  if (n < 0 || n >= path.size()) {
    return Point(-1, -1);
  }
  return path[n];
}

Point Path::operator[](int n)
{
  return step(n);
}

void Path::add_step(Point p, int cost)
{
  path.push_back(p);
  total_cost += cost;
}

void Path::erase_step(int index)
{
  if (index < 0 || index >= path.size()) {
    return;
  }
  path.erase(path.begin() + index);
}

void Path::clear()
{
  path.clear();
}

void Path::reverse()
{
  std::reverse(path.begin(), path.end());
}

void Path::offset(int x_offset, int y_offset)
{
  for (int i = 0; i < path.size(); i++) {
    path[i].x += x_offset;
    path[i].y += y_offset;
  }
}

Generic_map::Generic_map(int x, int y)
{
  set_size(x, y);
  x_offset = 0;
  y_offset = 0;
}

Generic_map::~Generic_map()
{
}

std::string Generic_map::save_data()
{
  std::stringstream ret;

  ret << get_size_x() << " ";
  ret << get_size_y() << " ";
  ret << x_offset << " " << y_offset << " ";
  ret << std::endl;

  for (int x = 0; x < get_size_x(); x++) {
    for (int y = 0; y < get_size_y(); y++) {
      ret << cost[x][y] << " ";
    }
  }

  return ret.str();
}

bool Generic_map::load_data(std::istream& data)
{
  int size_x, size_y;
  data >> size_x >> size_y;
  data >> x_offset >> y_offset;

  for (int x = 0; x < size_x; x++) {
    std::vector<int> tmpvec;
    for (int y = 0; y < size_y; y++) {
      int tmpval;
      data >> tmpval;
      tmpvec.push_back(tmpval);
    }
    cost.push_back(tmpvec);
  }

  return true;
}

void Generic_map::set_size(int x, int y)
{
  cost.clear();
  if (x == 0 && y == 0) {
    return;
  }
  std::vector<int> tmpvec;
  for (int i = 0; i < y; i++) {
    tmpvec.push_back(0);
  }
  for (int i = 0; i < x; i++) {
    cost.push_back( tmpvec );
  }
}

void Generic_map::set_cost(int x, int y, int c)
{
  if (x < 0 || x >= get_size_x() || y < 0 || y >= get_size_y()) {
    return;
  }
  cost[x][y] = c;
}

void Generic_map::set_cost(Point p, int c)
{
  set_cost(p.x, p.y, c);
}

int Generic_map::get_size_x()
{
  return cost.size();
}

int Generic_map::get_size_y()
{
  if (cost.empty()) {
    return 0;
  }
  return cost[0].size();
}

int Generic_map::get_cost(int x, int y)
{
  if (x < 0 || x >= get_size_x() || y < 0 || y >= get_size_y()) {
    return 0;
  }
  return cost[x][y];
}

int Generic_map::get_cost(Point p)
{
  return get_cost(p.x, p.y);
}

bool Generic_map::blocked(int x, int y)
{
  return (get_cost(x, y) < 0);
}

bool Generic_map::blocked(Point p)
{
  return blocked(p.x, p.y);
}

Pathfinder::Pathfinder()
{
  allow_diag = false;
  x_min  = 0;
  x_max  = 0;
  y_min  = 0;
  y_max  = 0;
  border = 0;
}

Pathfinder::Pathfinder(Generic_map m)
{
  allow_diag = false;
  set_map(m);
}

Pathfinder::~Pathfinder()
{
}

void Pathfinder::set_map(Generic_map m)
{
  map = m;
  set_bounds(0, 0, map.get_size_x() - 1, map.get_size_y() - 1);
}

void Pathfinder::set_bounds(int x0, int y0, int x1, int y1)
{

  if (x0 < 0) {
    x0 = 0;
  }
  if (x1 <= 0 || x1 >= map.get_size_x()) {
    x1 = map.get_size_x() - 1;
  }
  if (x0 > x1) {
    int tmp = x1;
    x1 = x0;
    x0 = tmp;
  }
  x_min = x0;
  x_max = x1;
  if (y0 < 0) {
    y0 = 0;
  }
  if (y1 <= 0 || y1 >= map.get_size_y()) {
    y1 = map.get_size_y() - 1;
  }
  if (y0 > y1) {
    int tmp = y1;
    y1 = y0;
    y0 = tmp;
  }
  y_min = y0;
  y_max = y1;
}

void Pathfinder::set_bounds(Point p0, Point p1)
{
  set_bounds(p0.x, p0.y, p1.x, p1.y);
}

void Pathfinder::set_bounds(int b)
{
  border = b;
}

void Pathfinder::set_allow_diagonal(bool allow)
{
  allow_diag = allow;
}

bool Pathfinder::in_bounds(int x, int y)
{
  return (x >= x_min && x >= 0 && x <= x_max && x < map.get_size_x() &&
          y >= y_min && y >= 0 && y <= y_max && y < map.get_size_y()   );
}

bool Pathfinder::in_bounds(Point p)
{
  return in_bounds(p.x, p.y);
}

Path Pathfinder::get_path(Path_type type, int x0, int y0, int x1, int y1)
{
  return get_path(type, Point(x0, y0), Point(x1, y1));
}

Path Pathfinder::get_path(Path_type type, Point start, Point end)
{
  switch (type) {

    case PATH_NULL:
    case PATH_LINE:
      return path_line(start, end);

    case PATH_A_STAR:
      return path_a_star(start, end);

    default:
      return path_line(start, end);
  }
  return path_line(start, end);
}

Point Pathfinder::get_step(Path_type type, int x0, int y0, int x1, int y1)
{
  return get_step(type, Point(x0, y0), Point(x1, y1));
}

Point Pathfinder::get_step(Path_type type, Point start, Point end)
{
  Path p = get_path(type, start, end);
  if (p.empty()) {
    return start;
  }
  return p[0];
}

Path Pathfinder::path_line(Point start, Point end)
{
  Path ret;
  Point cur = start;
  bool done = false;
  while (!done) {
    bool picked_next = false;
    if (cur == end) {
      done = true;
// Prioritize vertical movement over lateral
    } else {
      Point options[5];
      for (int i = 0; i < 5; i++) {
        options[i] = cur;
      }
      bool x_diff_bigger = ( abs(end.x - cur.x) > abs(end.y - cur.y) );
      int best_x_move = cur.x, alt_x_move = cur.x, worst_x_move = cur.x;;
      if (end.x > cur.x) {
        best_x_move++;
        worst_x_move--;
      } else if (end.x < cur.x) {
        best_x_move--;
        worst_x_move++;
      } else {
        int alt = 2 * rng(0, 1) - 1; // -1 or 1
        alt_x_move += alt;
        worst_x_move += -1 * alt;
      }
      int best_y_move = cur.y, alt_y_move = cur.y, worst_y_move = cur.y;;
      if (end.y > cur.y) {
        best_y_move++;
        worst_y_move--;
      } else if (end.y < cur.y) {
        best_y_move--;
        worst_y_move++;
      } else {
        int alt = 2 * rng(0, 1) - 1; // -1 or 1
        alt_y_move += alt;
        worst_y_move += -1 * alt;
      }
  
      options[0]   = Point(best_x_move,  best_y_move);
      if (x_diff_bigger) {
        options[1] = Point(best_x_move,  alt_y_move   );
        options[2] = Point(alt_x_move,   best_y_move  );
        options[3] = Point(best_x_move,  worst_y_move );
        options[4] = Point(worst_x_move, best_y_move  );
      } else {
        options[1] = Point(alt_x_move,   best_y_move  );
        options[2] = Point(best_x_move,  alt_y_move   );
        options[3] = Point(worst_x_move, best_y_move  );
        options[4] = Point(best_x_move,  worst_y_move );
      }
      for (int i = 0; i < 5 && !picked_next; i++) {
        if (!map.blocked( options[i] ) && in_bounds( options[i] ) &&
            (allow_diag || options[i].x == cur.x || options[i].y == cur.y)) {
          picked_next = true;
          cur = options[i];
        }
      }
    } // Lateral movement
    if (!picked_next) { // Couldn't reach our target using this stupid algo!
      done = true;
    } else {
      ret.add_step(cur, map.get_cost(cur));
    }
  } // while (!done)

  if (cur != end) { // We didn't make it :(
    return Path();
  }
  return ret;
}


enum A_star_status
{
  ASTAR_NONE,
  ASTAR_OPEN,
  ASTAR_CLOSED
};

Path Pathfinder::path_a_star(Point start, Point end)
{
  int x_size = map.get_size_x();
  int y_size = map.get_size_y();

  start.x -= map.x_offset;
  start.y -= map.y_offset;
  end.x   -= map.x_offset;
  end.y   -= map.y_offset;

  if (x_size == 0 || y_size == 0) {
    debugmsg("A* generated; %s => %s (size %d, %d)",
             start.str().c_str(), end.str().c_str(), x_size, y_size);
    return Path();
  }

  std::vector<Point> open_points;
  A_star_status status[x_size][y_size];
  int           gscore[x_size][y_size];
  int           hscore[x_size][y_size];
  Point         parent[x_size][y_size];

  if (border > 0) {
    int x0 = (start.x < end.x ? start.x : end.x);
    int y0 = (start.y < end.y ? start.y : end.y);
    int x1 = (start.x > end.x ? start.x : end.x);
    int y1 = (start.y > end.y ? start.y : end.y);

    set_bounds(x0 - border, y0 - border, x1 + border, y1 + border);
  }

// Init everything to 0
  for (int x = 0; x < x_size; x++) {
    for (int y = 0; y < y_size; y++) {
      status[x][y] = ASTAR_NONE;
      gscore[x][y] = 0;
      hscore[x][y] = 0;
      parent[x][y] = Point(-1, -1);
    }
  }

  status[start.x][start.y] = ASTAR_OPEN;
  open_points.push_back(start);

  bool done = false;


  while (!done && !open_points.empty()) {
// 1) Find the lowest cost in open_points, and set (current) to that point
// (if multiple points are tied, randomly select one)
    int lowest_cost = -1, point_index = -1;
    Point current;
    int current_g = 0;
    std::vector<int> lowest_indices;
    for (int i = 0; i < open_points.size(); i++) {
      Point p = open_points[i];
      int score = gscore[p.x][p.y] + hscore[p.x][p.y];
      if (i == 0 || score < lowest_cost) {
        lowest_cost = score;
        lowest_indices.clear();
        lowest_indices.push_back(i);
      } else if (score == lowest_cost) {
        lowest_indices.push_back(i);
      }
    }
    if (lowest_indices.empty()) { // Should never happen
      point_index = 0;
    } else {
      point_index = lowest_indices[ rng(0, lowest_indices.size() - 1) ];
    }
    current = open_points[point_index];
    current_g = gscore[current.x][current.y];
// 2) Check if (current) is the endpoint
    if (current == end) {
      done = true;
    } else {
// 3) Set (current) to be closed
      open_points.erase(open_points.begin() + point_index);
      status[current.x][current.y] = ASTAR_CLOSED;
// 4) Examine all adjacent points
      for (int x = current.x - 1; x <= current.x + 1; x++) {
        for (int y = current.y - 1; y <= current.y + 1; y++) {
          if (x == current.x && y == current.y) {
            y++; // Skip the current tile
          }
// If it's not diagonal, or diagonals are allowed...
// ...and if it's in-bounds and not blocked...
          if ((allow_diag || x == current.x || y == current.y) &&
              (in_bounds(x, y) && !map.blocked(x, y))) {
            int g = current_g + map.get_cost(x, y);
// If it's unexamined, make it open and set its values
            if (status[x][y] == ASTAR_NONE) {
              status[x][y] = ASTAR_OPEN;
              gscore[x][y] = g;
              if (allow_diag) {
                hscore[x][y] = map.get_cost(x, y) *
                                  rl_dist(x, y, end.x, end.y);
              } else {
                hscore[x][y] = map.get_cost(x, y) *
                                  manhattan_dist(x, y, end.x, end.y);
              }
              parent[x][y] = current;
              open_points.push_back( Point(x, y) );
// Otherwise, if it's open and we're a better parent, make us the parent
            } else if (status[x][y] == ASTAR_OPEN && g < gscore[x][y]) {
              gscore[x][y] = g;
              parent[x][y] = current;
            }
          } // if (non-diag or diag allowed; not blocked; and not OOB
        } // for (int y = current.y - 1; y <= current.y + 1; y++)
      } // for (int x = current.x - 1; x <= current.x + 1; x++)
    } // (current != end)
  } // while (!done && !open_points.empty())

  Path ret;
  if (open_points.empty()) {
    return ret; // No path exists!
  }
  Point cur = end;
  ret.add_step(cur, map.get_cost(cur));
  while (parent[cur.x][cur.y] != start) {
    cur = parent[cur.x][cur.y];
    ret.add_step(cur, map.get_cost(cur));
  }
  ret.reverse();
// Add the offsets back in.
  ret.offset(map.x_offset, map.y_offset);

  return ret;
}
