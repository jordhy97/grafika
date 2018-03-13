#include "font.h"
#include <iostream>
#include <fstream>

/* Constructor */
Font::Font(const char *file_path) {
  std::ifstream font_file;

  font_file.open(file_path);
  if (font_file.is_open()) {
    font_file >> width_ >> height_ >> horizontal_space_ >> vertical_space_;

    char character;
    int polygon_count;

    for (int i = 0; i < 26; i++) {
      font_file >> character >> polygon_count;
      for (int j = 0; j < polygon_count; j++) {
        int point_count;
        font_file >> point_count;
        int x, y;
        Polygon temp;
        for (int k = 0; k < point_count; k++) {
          font_file >> x >> y;
          temp.AddPoint(Point(x, y));
        }
        alphabets_[character - 'A'].push_back(temp);
      }
    }
    font_file.close();
  } else {
    perror("Error: failed to load font");
    exit(6);
  }
}

/* Render text */
void Font::RenderText(std::string text, const Font& font, Framebuffer& fb, const Point& start_position, const Color& border_color, const Color& fill_color, const Color& background_color, int scale, const Point& top_left, const Point& bottom_right) {
  for (unsigned int i = 0; i < text.length(); i++) {
    int xoffset = start_position.GetX() + i * (font.width_ * scale + font.horizontal_space_);
    if (text[i] != ' ') {
      int idx = text[i] - 'A';
      for (unsigned int j = 0; j < font.alphabets_[idx].size(); j++) {
        if (j == 0) {
          fb.DrawRasteredPolygon(Polygon::Scale(font.alphabets_[idx][j], Point(0, 0), scale), border_color, fill_color, top_left, bottom_right, xoffset, start_position.GetY());
        } else {
          fb.DrawRasteredPolygon(Polygon::Scale(font.alphabets_[idx][j], Point(0, 0), scale), border_color, background_color, top_left, bottom_right, xoffset, start_position.GetY());
        }
      }
    }
  }
}

/* Getter */
int Font::GetWidth() const {
  return width_;
}

int Font::GetHeight() const {
  return height_;
}

int Font::GetHorizontalSpace() const {
  return horizontal_space_;
}

int Font::GetVerticalSpace() const {
  return vertical_space_;
}
