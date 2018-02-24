#ifndef POLYGON_H
#define POLYGON_H

#include "point.h"
#include <vector>

class Polygon {
public:
	/* Constructor */
	Polygon() {}

  /* Getter */
  Point GetPoint(int idx) const { return points_[idx]; }
  int GetNumOfPoints() const { return points_.size(); }

  /* Setter */
  void AddPoint(const Point& point) { points_.push_back(point); }

private:
  /* Notes: the lines creating the polygon are consist of ordered points*/
	std::vector<Point> points_;
};

#endif
