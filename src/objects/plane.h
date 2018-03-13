#ifndef PLANE_H
#define PLANE_H

#include "../graphics/sprite.h"
#include "../graphics/point.h"
#include "../graphics/framebuffer.h"
#include "../graphics/color.h"

class Plane {
public:
	/* Constructor */
	Plane(const char *file_path, int y_speed = 0);

  /* Scale this plane */
  void Scale(double scale_factor);

  /* Render plane */
  void Render(Framebuffer& fb, const Point& top_left, const Point& bottom_right);

  /* Check whether the plane collided or not */
  bool IsCollide(const Framebuffer& fb, const Color& color);

  /* Getter */
  int GetYSpeed() const;
	Point GetCenter() const;
	Point GetTopLeft() const;
	Point GetBottomRight() const;

  /* Setter */
  void SetCenter(const Point& center);

private:
  Point top_left_;
  Point bottom_right_;
	Point center_;
  Sprite body_;
  int y_speed_;
};

#endif
