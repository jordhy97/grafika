#include "plane.h"
#include <iostream>
#include <fstream>

/* Constructor */
Plane::Plane(const char *file_path) {
  std::ifstream plane_file;

  plane_file.open(file_path);
  if (plane_file.is_open()) {
    int x, y;
    plane_file >> x >> y;
    center_ = Point(x, y);
    center_.Scale(Point(0, 0), INIT_SCALE);
    int point_count;
    plane_file >> point_count;
    for (int i = 0; i < point_count; i++) {
        plane_file >> x >> y;
        body_.AddPoint(Point::Scale(Point(x, y), Point(0, 0), INIT_SCALE));
    }
    for (int i = 0; i < 2; i++) {
      plane_file >> point_count;
      for (int j = 0; j < point_count; j++) {
        plane_file >> x >> y;
        mirrors_[i].AddPoint(Point::Scale(Point(x, y), Point(0, 0), INIT_SCALE));
      }
    }
    for (int i = 0; i < 2; i++) {
      plane_file >> point_count;
      for (int j = 0; j < point_count; j++) {
        plane_file >> x >> y;
        pilots_[i].AddPoint(Point::Scale(Point(x, y), Point(0, 0), INIT_SCALE));
      }
    }
    for (int i = 0; i < 2; i++) {
      plane_file >> point_count;
      for (int j = 0; j < point_count; j++) {
        plane_file >> x >> y;
        wings_[i].AddPoint(Point::Scale(Point(x, y), Point(0, 0), INIT_SCALE));
      }
    }
    for (int i = 0; i < 3; i++) {
      plane_file >> point_count;
      for (int j = 0; j < point_count; j++) {
        plane_file >> x >> y;
        tails_[i].AddPoint(Point::Scale(Point(x, y), Point(0, 0), INIT_SCALE));
      }
    }
    for (int i = 0; i < 8; i++) {
      plane_file >> point_count;
      for (int j = 0; j < point_count; j++) {
        plane_file >> x >> y;
        propellers_[i].AddPoint(Point::Scale(Point(x, y), Point(0, 0), INIT_SCALE));
      }
    }
    for (int i = 0; i < 3; i++) {
      plane_file >> point_count;
      for (int j = 0; j < point_count; j++) {
        plane_file >> x >> y;
        wheel_connectors_[i].AddPoint(Point::Scale(Point(x, y), Point(0, 0), INIT_SCALE));
      }
    }
    for (int i = 0; i < 3; i++) {
      plane_file >> point_count;
      for (int j = 0; j < point_count; j++) {
        plane_file >> x >> y;
        wheels_[i].AddPoint(Point::Scale(Point(x, y), Point(0, 0), INIT_SCALE));
      }
      wheel_fall_speed_[i] = INIT_FALL_SPEED;
      wheel_fall_limit_[i] = INIT_FALL_LIMIT;
    }
    plane_file.close();
    hit_counter_ = 0;
  } else {
    perror("Error: failed to load plane");
    exit(6);
  }
}

/* Scale this plane */
void Plane::Scale(double scale_factor) {
  body_.Scale(center_, scale_factor);
  for (int i = 0; i < 2; i++) {
    mirrors_[i].Scale(center_, scale_factor);
  }
  for (int i = 0; i < 2; i++) {
    pilots_[i].Scale(center_, scale_factor);
  }
  for (int i = 0; i < 2; i++) {
    wings_[i].Scale(center_, scale_factor);
  }
  for (int i = 0; i < 3; i++) {
    tails_[i].Scale(center_, scale_factor);
  }
  for (int i = 0; i < 8; i++) {
    propellers_[i].Scale(center_, scale_factor);
  }
  for (int i = 0; i < 3; i++) {
    wheel_connectors_[i].Scale(center_, scale_factor);
  }
  for (int i = hit_counter_; i < 3; i++) {
    wheels_[i].Scale(center_, scale_factor);
  }
}

/* Rotate plane propellers */
void Plane::RotatePropellers() {
  for (int i = 0; i < 8; i++) {
    propellers_[i].Rotate(propellers_[i].GetPoint(0), 45);
  }
}

/* Move plane wheels (when shot) */
void Plane::MoveWheels() {
  for (int i = 0; i < hit_counter_; i++) {
    if (wheel_fall_limit_[i] > 0) {
      if (wheel_fall_speed_[i] == wheel_fall_limit_[i] * INIT_FALL_SPEED) {
          wheel_fall_limit_[i]--;
          wheel_fall_speed_[i] *= -1;
          wheel_fall_speed_[i] += (2 * INIT_FALL_SPEED);
      }
      if (i == 2) {
        wheels_[i].Translate(Point(-5, wheel_fall_speed_[i]));
      } else {
        wheels_[i].Translate(Point(5, wheel_fall_speed_[i]));
      }
      wheel_fall_speed_[i] += INIT_FALL_SPEED;
    }
  }
}

/* Render plane */
void Plane::Render(Framebuffer& fb, const Point& top_left, const Point& bottom_right) {
  fb.DrawRasteredPolygon(body_, COLOR_RED, COLOR_RED, top_left, bottom_right, 0, 0);
  for (int i = 0; i < 2; i++) {
    fb.DrawRasteredPolygon(mirrors_[i], COLOR_WHITE, COLOR_WHITE, top_left, bottom_right, 0, 0);
  }
  for (int i = 0; i < 2; i++) {
    fb.DrawRasteredPolygon(pilots_[i], COLOR_BLACK, COLOR_BLACK, top_left, bottom_right, 0, 0);
  }
  for (int i = 0; i < 2; i++) {
    fb.DrawRasteredPolygon(wings_[i], COLOR_RED, COLOR_RED, top_left, bottom_right, 0, 0);
  }
  for (int i = 0; i < 3; i++) {
    fb.DrawRasteredPolygon(tails_[i], COLOR_RED, COLOR_RED, top_left, bottom_right, 0, 0);
  }
  for (int i = 0; i < 8; i++) {
    fb.DrawRasteredPolygon(propellers_[i], COLOR_WHITE, COLOR_WHITE, top_left, bottom_right, 0, 0);
  }
  for (int i = 0; i < 3; i++) {
    fb.DrawRasteredPolygon(wheel_connectors_[i], COLOR_RED, COLOR_RED, top_left, bottom_right, 0, 0);
  }
  for (int i = 0; i < 3; i++) {
    fb.DrawRasteredPolygon(wheels_[i], COLOR_WHITE, COLOR_BLACK, top_left, bottom_right, 0, 0);
  }
  MoveWheels();
}

/* Setter */
void Plane::SetCenter(const Point& center) {
  int xoffset = center.GetX() - center_.GetX();
  int yoffset = center.GetY() - center_.GetY();
  center_ = center;

  body_.Translate(Point(xoffset, yoffset));
  for (int i = 0; i < 2; i++) {
    mirrors_[i].Translate(Point(xoffset, yoffset));
  }
  for (int i = 0; i < 2; i++) {
    pilots_[i].Translate(Point(xoffset, yoffset));
  }
  for (int i = 0; i < 2; i++) {
    wings_[i].Translate(Point(xoffset, yoffset));
  }
  for (int i = 0; i < 3; i++) {
    tails_[i].Translate(Point(xoffset, yoffset));
  }
  for (int i = 0; i < 8; i++) {
    propellers_[i].Translate(Point(xoffset, yoffset));
  }
  for (int i = 0; i < 3; i++) {
    wheel_connectors_[i].Translate(Point(xoffset, yoffset));
  }
  for (int i = 0; i < 3; i++) {
    wheels_[i].Translate(Point(xoffset, yoffset));
  }
}

/* Increase hit counter */
void Plane::IncHitCounter() {
  hit_counter_++;
  if (hit_counter_ > 3) {
    hit_counter_ = 3;
  }
}

/* Check whether the plane is hit or not */
bool Plane::IsHit() {
  return hit_counter_ > 0;
}
