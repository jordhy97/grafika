#include "sprite.h"
#include <fstream>
#include <iostream>
#include <sstream>

/* Constructor */
Sprite::Sprite(const char *sprite_path) {
  std::ifstream sprite_file;
  char ignore_character;

  sprite_file.open(sprite_path);
  if (sprite_file.is_open()) {
    int polygon_count;
    sprite_file >> polygon_count;
    for (int i = 0; i < polygon_count; i++) {
      Polygon polygon;
      double dx, dy;
      double acc_x = 0.0;
      double acc_y = 0.0;
      unsigned int fill_r, fill_g, fill_b, border_r, border_g, border_b;
      std::string path_name;
      int count;
      sprite_file >> count;
      sprite_file >> fill_r >> fill_g >> fill_b;
      sprite_file >> border_r >> border_g >> border_b;
      sprite_file.ignore();

      for (int j = 0; j < count; j++) {
        std::string line;
        std::getline(sprite_file, line);
        std::istringstream closed_path(line);
        while(closed_path >> dx >> ignore_character >> dy) {
          acc_x += dx;
          acc_y += dy;
          polygon.AddPoint(Point(acc_x, acc_y));
        }
      }
      polygons_.push_back(polygon);
      fill_colors_.push_back(Color(fill_r, fill_g, fill_b));
      border_colors_.push_back(Color(border_r, border_g, border_b));
    }
    sprite_file.close();
  }
}

/* Translate this sprite */
Sprite& Sprite::Translate(const Point &p) {
  for (unsigned int i = 0; i < polygons_.size(); i++) {
    polygons_[i].Translate(p);
  }
  return *this;
}

/* Return sprite translated by p */
Sprite Sprite::Translate(const Sprite& sprite, const Point& p) {
  Sprite result = sprite;
  return result.Translate(p);
}

/* Rotate this sprite */
Sprite& Sprite::Rotate(const Point &pivot, double theta) {
  for (unsigned int i = 0; i < polygons_.size(); i++) {
    polygons_[i].Rotate(pivot, theta);
  }
  return *this;
}

/* Rotate this sprite */
Sprite Sprite::Rotate(const Sprite& sprite, const Point &pivot, double theta) {
  Sprite result = sprite;
  return result.Rotate(pivot, theta);
}

/* Scale this sprite */
Sprite& Sprite::Scale(const Point &pivot, double scale_factor) {
  for (unsigned int i = 0; i < polygons_.size(); i++) {
    polygons_[i].Scale(pivot, scale_factor);
  }
  return *this;
}
Sprite& Sprite::Scale(const Point &pivot, double x_scale_factor, double y_scale_factor) {
  for (unsigned int i = 0; i < polygons_.size(); i++) {
    polygons_[i].Scale(pivot, x_scale_factor, y_scale_factor);
  }
  return *this;
}

/* Return the scaled sprite */
Sprite Sprite::Scale(const Sprite& sprite, const Point &pivot, double scale_factor) {
  Sprite result = sprite;
  return result.Scale(pivot, scale_factor);
}

Sprite Sprite::Scale(const Sprite& sprite, const Point &pivot, double x_scale_factor, double y_scale_factor) {
  Sprite result = sprite;
  return result.Scale(pivot, x_scale_factor, y_scale_factor);
}
