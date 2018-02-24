#ifndef PILOT_H
#define PILOT_H

#define PILOT_X_SPEED -15

#include "../graphics/polygon.h"
#include "../graphics/point.h"
#include "../graphics/framebuffer.h"

class Pilot {
public:
	/* Constructor */
	Pilot(const char *file_path, int y_speed);

  /* Scale this pilot */
  void Scale(double scale_factor);

  /* Render pilot */
  void Render(Framebuffer& fb);

  /* Setter */
  void SetCenter(const Point& center);

	/* Move pilot */
	void Move();

private:
	Point center_;
  Polygon pilot_;
  Polygon parachute_;
  Polygon parachute_strings_[4];
  int y_speed_;
};

#endif
