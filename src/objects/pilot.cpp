#include "pilot.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>

/* Constructor */
Pilot::Pilot(const char *file_path, int y_speed) {
  std::ifstream pilot_file;

  pilot_file.open(file_path);
  if (pilot_file.is_open()) {
    int x, y;
    pilot_file >> x >> y;
    center_ = Point(x, y);
    int point_count;
    pilot_file >> point_count;
    for (int i = 0; i < point_count; i++) {
        pilot_file >> x >> y;
        pilot_.AddPoint(Point(x, y));
    }
    pilot_file >> point_count;
    for (int i = 0; i < point_count; i++) {
        pilot_file >> x >> y;
        parachute_.AddPoint(Point(x, y));
    }
    for (int i = 0; i < 4; i++) {
      pilot_file >> point_count;
      for (int j = 0; j < point_count; j++) {
        pilot_file >> x >> y;
        parachute_strings_[i].AddPoint(Point(x, y));
      }
    }
    pilot_file.close();
    y_speed_ = y_speed;
  } else {
    perror("Error: failed to load plane");
    exit(6);
  }
}

/* Scale this pilot */
void Pilot::Scale(double scale_factor) {
  pilot_.Scale(center_, scale_factor);
  parachute_.Scale(center_, scale_factor);
  for (int i = 0; i < 4; i++) {
    parachute_strings_[i].Scale(center_, scale_factor);
  }
}

/* Render pilot */
void Pilot::Render(Framebuffer& fb, const Point& top_left, const Point& bottom_right) {
  Move();
  fb.DrawRasteredPolygon(pilot_, COLOR_BLACK, COLOR_BLACK, top_left, bottom_right, 0, 0);
  if (y_speed_ > 0) {
    fb.DrawRasteredPolygon(parachute_, COLOR_WHITE, COLOR_YELLOW, top_left, bottom_right, 0, 0);
    for (int i = 0 ; i < 4; i++) {
      fb.DrawRasteredPolygon(parachute_strings_[i], COLOR_WHITE, COLOR_WHITE, top_left, bottom_right, 0, 0);
  }
}
}

/* Setter */
void Pilot::SetCenter(const Point& center) {
  int xoffset = center.GetX() - center_.GetX();
  int yoffset = center.GetY() - center_.GetY();
  center_ = center;

  pilot_.Translate(Point(xoffset, yoffset));
  parachute_.Translate(Point(xoffset, yoffset));
  for (int i = 0; i < 4; i++) {
    parachute_strings_[i].Translate(Point(xoffset, yoffset));
  }
}

/* Move pilot */
void Pilot::Move() {
  int x_offset;

  if (y_speed_ > 0) {
    y_speed_ = 2;
    x_offset = rand() % 11 - 5;
  } else {
    x_offset = PILOT_X_SPEED;
  }
  pilot_.Translate(Point(x_offset, y_speed_));
  parachute_.Translate(Point(x_offset, y_speed_));
  for (int i = 0; i < 4; i++) {
    parachute_strings_[i].Translate(Point(x_offset, y_speed_));
  }
  y_speed_ += 5;
}
