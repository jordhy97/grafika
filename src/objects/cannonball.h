#ifndef CANNON_BALL_H
#define CANNON_BALL_H

#define CANNON_BALL_INIT_SCALE 4
#define SPEED_RATE 3

#include "../graphics/polygon.h"
#include "../graphics/point.h"
#include "../graphics/framebuffer.h"


class CannonBall {
public:
	/* Constructor */
	CannonBall(const char *file_path, int x_speed, int y_speed);

  /* Scale this cannon ball */
  void Scale(double scale_factor);

  /* Render cannon ball */
  void Render(Framebuffer& fb, const Point& top_left, const Point& bottom_right);

  /* Setter */
  void SetCenter(const Point& center);
	void SetDestroyed(bool destroyed);

	/* Move cannon ball */
	void Move();

	/* Getter */
	bool IsDestroyed() const;

	int x_min_;
  int x_max_;
  int y_min_;
  int y_max_;

private:
	Point center_;
  Polygon cannon_ball_;
  int y_speed_;
  int x_speed_;
  int counter_;
  bool destroyed_;
};

#endif
