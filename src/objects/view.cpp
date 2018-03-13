#include "view.h"

/* Constructor */
View::View(const Point& top_left, const Point& bottom_right, const Color& border_color) {
  top_left_ = top_left;
  bottom_right_ = bottom_right;
  border_color_ = border_color;
}

/* Setter */
void View::AddSource(Sprite *sprite) {
  sources_.push_back(sprite);
  is_source_visible_.push_back(true);
}

void View::SetSourcePosition(const Point& source_top_left, const Point& source_bottom_right) {
  source_top_left_ = source_top_left;
  source_bottom_right_ = source_bottom_right;
}

void View::SetVisible(int idx) {
  is_source_visible_[idx] = !is_source_visible_[idx];
}

/* Render this view */
void View::Render(Framebuffer& fb) {
  /* Render sources */
  double x_scale_factor = 600;
  double y_scale_factor = 600;
  if (sources_.size() > 0) {
    if (source_bottom_right_.GetX() != source_top_left_.GetX()) {
      x_scale_factor = (double)(bottom_right_.GetX() - top_left_.GetX()) / (double)(source_bottom_right_.GetX() - source_top_left_.GetX());
    }
    if (source_bottom_right_.GetY() != source_top_left_.GetY()) {
      y_scale_factor = (double)(bottom_right_.GetY() - top_left_.GetY()) / (double)(source_bottom_right_.GetY() - source_top_left_.GetY());
    }
    for (unsigned int i = 0; i < sources_.size(); i++) {
      if (is_source_visible_[i]) {
        Sprite sprite = Sprite::Translate(Sprite::Scale((*sources_[i]), source_top_left_, x_scale_factor, y_scale_factor),
          Point(top_left_.GetX() - source_top_left_.GetX(), top_left_.GetY() - source_top_left_.GetY()));
        fb.DrawClippedSprite(sprite, top_left_, bottom_right_);
      }
    }
  }

  /* Render border */
  fb.DrawLine(top_left_, Point(bottom_right_.GetX(), top_left_.GetY()), border_color_);
  fb.DrawLine(top_left_, Point(top_left_.GetX(), bottom_right_.GetY()), border_color_);
  fb.DrawLine(bottom_right_, Point(bottom_right_.GetX(), top_left_.GetY()), border_color_);
  fb.DrawLine(bottom_right_, Point(top_left_.GetX(), bottom_right_.GetY()), border_color_);
}
