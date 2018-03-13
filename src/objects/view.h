#ifndef VIEW_H
#define VIEW_H

#include "../graphics/framebuffer.h"
#include "../graphics/sprite.h"
#include "../graphics/point.h"
#include "../graphics/color.h"

#include <vector>

class View {
public:
	/* Constructor */
	View(const Point& top_left, const Point& bottom_right, const Color& border_color);

  /* Setter */
  void AddSource(Sprite *sprite);
  void SetSourcePosition(const Point& source_top_left, const Point& source_bottom_right);
  void SetVisible(int idx);

  /* Render this view */
  void Render(Framebuffer& fb);

private:
  Point top_left_;
  Point bottom_right_;
  Point source_top_left_;
  Point source_bottom_right_;
  Color border_color_;
  std::vector<Sprite*> sources_;
  std::vector<bool> is_source_visible_;
};

#endif
