#include "point.h"

/* Constructor */
Point::Point() : x_(0), y_(0) {}

Point::Point(int x, int y): x_(x), y_(y) {}

/* Returns this point translated by p */
Point Point::Translate(const Point& p1, const Point& p2) {
  return Point(p1.GetX() + p2.GetX(), p1.GetY() + p2.GetY());
}

/* Translate this point by p */
void Point::Translate(const Point& p) {
  x_ = x_ + p.GetX();
  y_ = y_ + p.GetY();
}

/* Return this point scaled by scale factor with the specified pivot */
Point Point::Scale(const Point& p, const Point& pivot, double scale_factor) {
  return Point(pivot.GetX() + (p.GetX() - pivot.GetX()) * scale_factor,
    pivot.GetY() + (p.GetY() - pivot.GetY()) * scale_factor);
}

/* Scale this point by scale factor with the specified pivot */
void Point::Scale(const Point& pivot, double scale_factor) {
  x_ = pivot.GetX() + (x_ - pivot.GetX()) * scale_factor;
  y_ = pivot.GetY() + (y_ - pivot.GetY()) * scale_factor;
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
