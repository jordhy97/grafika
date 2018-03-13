#ifndef SPRITE_H
#define SPRITE_H

#include "polygon.h"
#include "color.h"
#include <vector>

class Sprite {
public:
	/* Constructor */
	Sprite() {}
	Sprite(const char *sprite_path);

	/* Translate this sprite */
	Sprite& Translate(const Point &p);

	/* Return sprite translated by p */
	static Sprite Translate(const Sprite& sprite, const Point& p);

	/* Rotate this sprite */
	Sprite& Rotate(const Point &pivot, double theta);

	/* Rotate this sprite */
	static Sprite Rotate(const Sprite& sprite, const Point &pivot, double theta);


	/* Scale this sprite */
	Sprite& Scale(const Point &pivot, double scale_factor);
	Sprite& Scale(const Point &pivot, double x_scale_factor, double y_scale_factor);

	/* Return the scaled sprite */
	static Sprite Scale(const Sprite& sprite, const Point &pivot, double scale_factor);
	static Sprite Scale(const Sprite& sprite, const Point &pivot, double x_scale_factor, double y_scale_factor);

  std::vector<Polygon> polygons_;
  std::vector<Color> fill_colors_;
  std::vector<Color> border_colors_;
};

#endif
