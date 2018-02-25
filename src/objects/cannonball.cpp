#include "cannonball.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>

/* Constructor */
CannonBall::CannonBall(const char *file_path, int x_speed, int y_speed) {
  std::ifstream cannon_ball_file;

  cannon_ball_file.open(file_path);
  if (cannon_ball_file.is_open()) {
    x_min_ = 10000;
    y_min_ = 10000;
    x_max_ = 0;
    y_max_ = 0;

    int x, y;
    cannon_ball_file >> x >> y;
    center_ = Point(x, y);
    center_.Scale(Point(0, 0), CANNON_BALL_INIT_SCALE);
    int point_count;
    cannon_ball_file >> point_count;
    for (int i = 0; i < point_count; i++) {
      cannon_ball_file >> x >> y;
      cannon_ball_.AddPoint(Point::Scale(Point(x, y), Point(0, 0), CANNON_BALL_INIT_SCALE));
      if (cannon_ball_.GetPoint(i).GetX() < x_min_) {
          x_min_ = cannon_ball_.GetPoint(i).GetX();
      }
      if (cannon_ball_.GetPoint(i).GetX() > x_max_) {
          x_max_ = cannon_ball_.GetPoint(i).GetX();
      }
      if (cannon_ball_.GetPoint(i).GetY() < y_min_) {
          y_min_ = cannon_ball_.GetPoint(i).GetY();
      }
      if (cannon_ball_.GetPoint(i).GetY() > y_max_) {
          y_max_ = cannon_ball_.GetPoint(i).GetY();
      }
    }
    cannon_ball_file.close();
    x_speed_ = x_speed;
    y_speed_ = y_speed;
    counter_ = 0;
    destroyed_ = false;
  } else {
    perror("Error: failed to load plane");
    exit(6);
  }
}

/* Scale this cannon ball */
void CannonBall::Scale(double scale_factor) {
  cannon_ball_.Scale(center_, scale_factor);
  x_min_ = center_.GetX() + (x_min_ - center_.GetX()) * scale_factor;
  x_max_ = center_.GetX() + (x_max_ - center_.GetX()) * scale_factor;
  y_min_ = center_.GetY() + (y_min_ - center_.GetY()) * scale_factor;
  y_max_ = center_.GetY() + (y_max_ - center_.GetY()) * scale_factor;
}

/* Render cannon ball */
void CannonBall::Render(Framebuffer& fb, const Point& top_left, const Point& bottom_right) {
  Move();
  fb.DrawRasteredPolygon(cannon_ball_, COLOR_GRAY, COLOR_GRAY, top_left, bottom_right, 0, 0);
}

/* Setter */
void CannonBall::SetCenter(const Point& center) {
  int xoffset = center.GetX() - center_.GetX();
  int yoffset = center.GetY() - center_.GetY();
  center_ = center;

  cannon_ball_.Translate(Point(xoffset, yoffset));
  x_min_ += xoffset;
  x_max_ += xoffset;
  y_min_ += yoffset;
  y_max_ += yoffset;
}

void CannonBall::SetDestroyed(bool destroyed) {
  destroyed_ = destroyed;
}

/* Move cannon ball */
void CannonBall::Move() {
  counter_++;
  if (counter_ == 35) {
    destroyed_ = true;
  } else {
    center_.Translate(Point(x_speed_, y_speed_));
    cannon_ball_.Translate(Point(x_speed_, y_speed_));
    if (counter_ % 10 == 0) {
       Scale(0.5);
     }
     y_speed_ += SPEED_RATE;
     x_min_ += x_speed_;
     x_max_ += x_speed_;
     y_min_ += y_speed_;
     y_max_ += y_speed_;
 }
}

/* Getter */
bool CannonBall::IsDestroyed() const {
  return destroyed_;
}
