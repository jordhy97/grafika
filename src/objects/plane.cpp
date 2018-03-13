#include "plane.h"
#include <iostream>
#include <fstream>

/* Constructor */
Plane::Plane(const char *file_path, int y_speed) {
  std::ifstream plane_file;

  plane_file.open(file_path);
  if (plane_file.is_open()) {
    int x, y;

    plane_file >> x >> y;
    top_left_ = Point(x, y);

    plane_file >> x >> y;
    bottom_right_ = Point(x, y);

    plane_file >> x >> y;
    center_ = Point(x, y);

    int polygon_count;
    plane_file >> polygon_count;

    unsigned int fill_r, fill_g, fill_b, border_r, border_g, border_b;
    plane_file >> fill_r >> fill_g >> fill_b;
    plane_file >> border_r >> border_g >> border_b;

    for (int i = 0; i < polygon_count; i++) {
        int point_count;
        plane_file >> point_count;

        Polygon polygon;
        for (int j = 0; j < point_count; j++) {
          plane_file >> x >> y;
          polygon.AddPoint(Point(x, y));
        }
        body_.polygons_.push_back(polygon);
        body_.fill_colors_.push_back(Color(fill_r, fill_g, fill_b));
        body_.border_colors_.push_back(Color(border_r, border_g, border_b));
    }
    plane_file.close();
    y_speed_ = y_speed;
  } else {
    perror("Error: failed to load plane");
    exit(6);
  }
}

/* Scale this plane */
void Plane::Scale(double scale_factor) {
  body_.Scale(center_, scale_factor);
  top_left_.Scale(center_, scale_factor);
  bottom_right_.Scale(center_, scale_factor);
}

/* Render plane */
void Plane::Render(Framebuffer& fb, const Point& top_left, const Point& bottom_right) {
  fb.DrawClippedSprite(body_, top_left, bottom_right);
}

/* Setter */
void Plane::SetCenter(const Point& center) {
  int xoffset = center.GetX() - center_.GetX();
  int yoffset = center.GetY() - center_.GetY();
  center_ = center;

  body_.Translate(Point(xoffset, yoffset));
  top_left_.Translate(Point(xoffset, yoffset));
  bottom_right_.Translate(Point(xoffset, yoffset));
}

/* Getter */
int Plane::GetYSpeed() const {
  return y_speed_;
}

Point Plane::GetCenter() const {
    return center_;
}

Point Plane::GetTopLeft() const {
  return top_left_;
}

Point Plane::GetBottomRight() const {
  return bottom_right_;
}

/* Check whether the plane collided or not */
bool Plane::IsCollide(const Framebuffer& fb, const Color& color) {
  for (int i = top_left_.GetY(); i <= bottom_right_.GetY(); i++) {
    for (int j = top_left_.GetX(); j <= bottom_right_.GetX(); j++) {
      if (Color::IsColorSame(fb.GetPixelColor(Point(j, i)), color)) {
        return true;
      }
    }
  }
  return false;
}
