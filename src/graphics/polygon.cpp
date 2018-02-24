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
void Polygon::Translate(const Point &p) {
  for (unsigned int i = 0; i < points_.size(); i++) {
    points_[i].Translate(p);
  }
}

/* Rotate this polygon */
void Polygon::Rotate(const Point &pivot, double theta) {
  for (unsigned int i = 0; i < points_.size(); i++) {
    points_[i].Rotate(pivot, theta);
  }
}

/* Scale this polygon */
void Polygon::Scale(const Point &pivot, double scale_factor) {
  for (unsigned int i = 0; i < points_.size(); i++) {
    points_[i].Scale(pivot, scale_factor);
  }
}

/* Setter */
void Polygon::AddPoint(const Point& point) {
  points_.push_back(point);
}

void Polygon::SetPoint(const Point& point, int idx) {
  points_[idx] = point;
}
