#include "polygon.h"

/* Constructor */
Polygon::Polygon() {}

/* Getter */
Point Polygon::GetPoint(int idx) const {
  return points_[idx];
}

int Polygon::GetNumOfPoints() const {
  return points_.size();
}

/* Translate this polygon */
Polygon& Polygon::Translate(const Point &p) {
  for (unsigned int i = 0; i < points_.size(); i++) {
    points_[i].Translate(p);
  }
  return *this;
}

/* Returns polygon translated by p */
Polygon Polygon::Translate(const Polygon& polygon, const Point& p) {
  Polygon result;
  for (unsigned int i = 0; i < polygon.points_.size(); i++) {
    result.AddPoint(Point::Translate(polygon.points_[i], p));
  }
  return result;
}

/* Rotate this polygon */
Polygon& Polygon::Rotate(const Point &pivot, double theta) {
  for (unsigned int i = 0; i < points_.size(); i++) {
    points_[i].Rotate(pivot, theta);
  }
  return *this;
}


/* Rotate this polygon */
Polygon Polygon::Rotate(const Polygon& polygon, const Point &pivot, double theta) {
  Polygon result;
  for (unsigned int i = 0; i < polygon.points_.size(); i++) {
    result.AddPoint(Point::Rotate(polygon.points_[i], pivot, theta));
  }
  return result;
}


/* Scale this polygon */
Polygon& Polygon::Scale(const Point &pivot, double scale_factor) {
  for (unsigned int i = 0; i < points_.size(); i++) {
    points_[i].Scale(pivot, scale_factor);
  }
  return *this;
}
Polygon& Polygon::Scale(const Point &pivot, double x_scale_factor, double y_scale_factor) {
  for (unsigned int i = 0; i < points_.size(); i++) {
    points_[i].Scale(pivot, x_scale_factor, y_scale_factor);
  }
  return *this;
}

	/* Returns the scaled polygon */
Polygon Polygon::Scale(const Polygon& polygon, const Point &pivot, double scale_factor) {
  Polygon result;
  for (unsigned int i = 0; i < polygon.points_.size(); i++) {
    result.AddPoint(Point::Scale(polygon.points_[i], pivot, scale_factor));
  }
  return result;
}

Polygon Polygon::Scale(const Polygon& polygon, const Point &pivot, double x_scale_factor, double y_scale_factor) {
  Polygon result;
  for (unsigned int i = 0; i < polygon.points_.size(); i++) {
    result.AddPoint(Point::Scale(polygon.points_[i], pivot, x_scale_factor, y_scale_factor));
  }
  return result;
}

/* Setter */
void Polygon::AddPoint(const Point& point) {
  points_.push_back(point);
}

void Polygon::SetPoint(const Point& point, int idx) {
  points_[idx] = point;
}
