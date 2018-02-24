#include "point.h"
#include <cmath>

/* Constructor */
Point::Point() : x_(0), y_(0) {}

Point::Point(int x, int y): x_(x), y_(y) {}

/* Returns this point translated by p */
Point Point::Translate(const Point& p1, const Point& p2) {
  return Point(p1.GetX() + p2.GetX(), p1.GetY() + p2.GetY());
}

/* Translate this point by p */
Point& Point::Translate(const Point& p) {
  x_ = x_ + p.GetX();
  y_ = y_ + p.GetY();
  return *this;
}

/* Return this point scaled by scale factor with the specified pivot */
Point Point::Scale(const Point& p, const Point& pivot, double scale_factor) {
  return Point(pivot.GetX() + (p.GetX() - pivot.GetX()) * scale_factor,
    pivot.GetY() + (p.GetY() - pivot.GetY()) * scale_factor);
}

/* Scale this point by scale factor with the specified pivot */
Point& Point::Scale(const Point& pivot, double scale_factor) {
  x_ = pivot.GetX() + (x_ - pivot.GetX()) * scale_factor;
  y_ = pivot.GetY() + (y_ - pivot.GetY()) * scale_factor;
  return *this;
}

/* Return p rotated by theta degree with the specified pivot */
Point Point::Rotate(const Point& p, const Point& pivot, double theta) {
  Point result;
  double cos_theta, sin_theta;

  cos_theta = std::cos(theta * PI / 180);
  sin_theta = std::sin(theta * PI / 180);

  result.SetX(pivot.GetX() + (p.GetX() - pivot.GetX() * cos_theta) - (p.GetY() - pivot.GetY()) * sin_theta);
  result.SetY(pivot.GetY() + (p.GetY() - pivot.GetY() * cos_theta) + (p.GetX() - pivot.GetX()) * sin_theta);
  return result;
}

/* Rotate this point by thetha degree with the specified pivot */
Point& Point::Rotate(const Point& pivot, double theta) {
  double cos_theta, sin_theta;

  cos_theta = std::cos(theta * PI / 180);
  sin_theta = std::sin(theta * PI / 180);

  SetX(pivot.GetX() + (x_ - pivot.GetX() * cos_theta) - (y_ - pivot.GetY()) * sin_theta);
  SetY(pivot.GetY() + (y_ - pivot.GetY() * cos_theta) + (x_ - pivot.GetX()) * sin_theta);
  return *this;
}

/* Getter */
int Point::GetX() const {
  return x_;
}

int Point::GetY() const {
  return y_;
}

/* Setter */
void Point::SetX(int x) {
  x_ = x;
}

void Point::SetY(int y) {
  y_ = y;
}
