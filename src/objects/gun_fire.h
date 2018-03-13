#ifndef GUN_FIRE_H
#define GUN_FIRE_H

#include "../graphics/point.h"
#include "../graphics/framebuffer.h"
#include "../graphics/color.h"

class GunFire {
public:
	/* Constructor */
	GunFire(const Point& start, const Point& end, const Color& color, int y_speed);

  /* Render gun fire */
  void Render(Framebuffer& fb, const Point& top_left, const Point& bottom_right);

  /* Move gun fire */
  void Move();

  /* Getter */
  Point GetStart() const;
  Point GetEnd() const;

private:
  Point start_;
  Point end_;
  Color color_;
  int y_speed_;
};

#endif
