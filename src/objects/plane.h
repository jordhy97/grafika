#ifndef PLANE_H
#define PLANE_H

#define INIT_SCALE 3
#define INIT_FALL_SPEED 5
#define INIT_FALL_LIMIT 10

#include "../graphics/polygon.h"
#include "../graphics/point.h"
#include "../graphics/framebuffer.h"

class Plane {
public:
	/* Constructor */
	Plane(const char *file_path);

  /* Scale this plane */
  void Scale(double scale_factor);

  /* Rotate plane propellers */
  void RotatePropellers();

  /* Move plane wheels (when shot) */
  void MoveWheels();

  /* Render plane */
  void Render(Framebuffer& fb);

  /* Setter */
  void SetCenter(const Point& center);

	/* Increase hit counter */
	void IncHitCounter();

	/* Check whether the plane is hit or not */
	bool IsHit();

private:
	Point center_;
  Polygon body_;
  Polygon mirrors_[2];
  Polygon pilots_[2];
  Polygon wings_[2];
  Polygon tails_[3];
  Polygon propellers_[8];
  Polygon wheel_connectors_[3];
  Polygon wheels_[3];
  int hit_counter_;
  int wheel_fall_speed_[3];
  int wheel_fall_limit_[3];
};

#endif
