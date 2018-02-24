#ifndef POLYGON_H
#define POLYGON_H

#include "point.h"
#include <vector>

class Polygon {
public:
	/* Constructor */
	Polygon();

  /* Getter */
  Point GetPoint(int idx) const;
  int GetNumOfPoints() const;

	/* Translate this polygon */
	void Translate(const Point &p);

	/* Rotate this polygon */
	void Rotate(const Point &pivot, double theta);

	/* Scale this polygon */
	void Scale(const Point &pivot, double scale_factor);

  /* Setter */
  void AddPoint(const Point& point);
	void SetPoint(const Point& point, int idx);

private:
  /* Notes: the lines creating the polygon are consist of ordered points*/
	std::vector<Point> points_;
};

#endif
