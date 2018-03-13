#ifndef FONT_H
#define FONT_H

#include "../graphics/polygon.h"
#include "../graphics/framebuffer.h"
#include "../graphics/color.h"
#include <string>
#include <vector>

class Font {
public:
	/* Constructor */
	Font(const char *file_path);

	/* Render text */
	static void RenderText(std::string text, const Font& font, Framebuffer& fb, const Point& start_position, const Color& border_color, const Color& fill_color, const Color& background_color, int scale, const Point& top_left, const Point& bottom_right);

	/* Getter */
	int GetWidth() const;
	int GetHeight() const;
	int GetHorizontalSpace() const;
	int GetVerticalSpace() const;

private:
	int height_;
	int width_;
	int horizontal_space_;
	int vertical_space_;
  std::vector<Polygon> alphabets_[26];
};

#endif
