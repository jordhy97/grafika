#include "gun_fire.h"

/* Constructor */
GunFire::GunFire(const Point& start, const Point& end, const Color& color, int y_speed) {
  start_ = start;
  end_ = end;
  color_ = color;
  y_speed_ = y_speed;
}

/* Render gun fire */
void GunFire::Render(Framebuffer& fb, const Point& top_left, const Point& bottom_right) {
  fb.ClipLine(start_, end_, top_left, bottom_right, color_);
}

/* Move gun fire */
void GunFire::Move() {
  start_.Translate(Point(0, y_speed_));
  end_.Translate(Point(0, y_speed_));
}

/* Getter */
Point GunFire::GetStart() const {
  return start_;
}
Point GunFire::GetEnd() const {
  return end_;
}
