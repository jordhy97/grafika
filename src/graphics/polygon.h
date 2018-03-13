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
	Polygon& Translate(const Point &p);

	/* Returns polygon translated by p */
	static Polygon Translate(const Polygon& polygon, const Point& p);

	/* Rotate this polygon */
	Polygon& Rotate(const Point &pivot, double theta);

	/* Rotate this polygon */
	static Polygon Rotate(const Polygon& polygon, const Point &pivot, double theta);

	/* Scale this polygon */
	Polygon& Scale(const Point &pivot, double scale_factor);
	Polygon& Scale(const Point &pivot, double x_scale_factor, double y_scale_factor);

	/* Returns the scaled polygon */
	static Polygon Scale(const Polygon& polygon, const Point &pivot, double scale_factor);
	static Polygon Scale(const Polygon& polygon, const Point &pivot, double x_scale_factor, double y_scale_factor);

  /* Setter */
  void AddPoint(const Point& point);
	void SetPoint(const Point& point, int idx);

private:
  /* Notes: the lines creating the polygon are consist of ordered points*/
	std::vector<Point> points_;
};

#endif
