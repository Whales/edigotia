#ifndef _PATHFIND_H_
#define _PATHFIND_H_

#include "geometry.h"
#include <vector>
#include <string> // For Generic_map::save_data()
#include <istream> // For Generic_map::load_data()

enum Path_type
{
  PATH_NULL = 0,
  PATH_LINE,
  PATH_A_STAR,
};

class Path
{
public:
  Path();
  ~Path();

  std::vector<Point> get_points();
  int get_cost();

  Point step(int n);
  Point operator[](int n);
  int  size()  { return path.size(); }
  bool empty() { return size() == 0; }

  void add_step(Point p, int cost);
  void erase_step(int index);
  void clear();
  void reverse();
  void offset(int x_offset, int y_offset);

private:
  std::vector<Point> path;
  int total_cost;
};

class Generic_map
{
public:
  Generic_map(int x = 0, int y = 0);
  ~Generic_map();

  std::string save_data();
  bool load_data(std::istream& data);

// set_size() also clears the map fully.
  void set_size(int x, int y);
  void set_offset(int x, int y);
  void set_cost(int x, int y, int c);
  void set_cost(Point p, int c);

  int  get_size_x();
  int  get_size_y();

  int  get_cost(int x, int y);
  int  get_cost(Point p);

  bool blocked(int x, int y);
  bool blocked(Point p);

  int x_offset, y_offset;

private:
  std::vector< std::vector<int> > cost;
};

class Pathfinder
{
public:
  Pathfinder();
  Pathfinder(Generic_map m);
  ~Pathfinder();

  void set_map(Generic_map m);

  void set_bounds(int x0, int y0, int x1, int y1);
  void set_bounds(Point p0, Point p1);
  void set_bounds(int b); // As a "border" around the square formed by the
                          // start/end points

  void set_allow_diagonal(bool allow = true);

  bool in_bounds(int x, int y);
  bool in_bounds(Point p);
  
  Path get_path(Path_type type, int x0, int y0, int x1, int y1);
  Path get_path(Path_type type, Point start, Point end);

// Just returns the first step in the path; useful when the path is likely to
// change often (e.g. for monster pathfinding)
  Point get_step(Path_type type, int x0, int y0, int x1, int y1);
  Point get_step(Path_type type, Point start, Point end);

  Generic_map map;

private:
  int x_min, x_max, y_min, y_max;
  int border;
  bool allow_diag;

  Path path_line  (Point start, Point end);
  Path path_a_star(Point start, Point end);
};

#endif
