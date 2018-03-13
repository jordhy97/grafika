#include "framebuffer.h"
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <algorithm>

/* Constructor */
Framebuffer::Framebuffer(const char *device_path) {
  // Open framebuffer device for reading and writing
	device_ = open(device_path, O_RDWR);
  if (device_ == -1) {
    perror("Error: cannot open framebuffer device");
    exit(1);
  }

  // Read fixed screen information
  if (ioctl(device_, FBIOGET_FSCREENINFO, &finfo_) == -1) {
    perror("Error: failed to read fixed screen information");
    exit(2);
  }

	// Read and write settings to variable screen information
	if (ioctl(device_, FBIOGET_VSCREENINFO, &vinfo_) == -1) {
    perror("Error: failed to read variable screen information");
    exit(3);
  }
	vinfo_.grayscale = 0;
	vinfo_.bits_per_pixel = 32;
	vinfo_.xoffset = 0;
	vinfo_.yoffset = 0;
	if (ioctl(device_, FBIOPUT_VSCREENINFO, &vinfo_) == -1) {
    perror("Error: failed to write variable screen information");
    exit(4);

  }

  // Calculate screen memory size in bytes
	screen_memory_size_ = vinfo_.yres_virtual * finfo_.line_length;

  // Map the device to memory
  address_ = (uint8_t*) mmap(0, screen_memory_size_, PROT_READ | PROT_WRITE, MAP_SHARED, device_, 0);
  if (!address_) {
    perror("Error: failed to map framebuffer device to memory");
    exit(5);
  }

  buffer_ = new uint8_t[screen_memory_size_];
}

/* Destructor */
Framebuffer::~Framebuffer() {
  delete[] buffer_;
  munmap(address_, screen_memory_size_);
  close(device_);
}

/* Set a pixel with specified color to the specified point in framebuffer */
void Framebuffer::SetPixel(const Point& position, const Color& color) {
  long int address_offset;
  if ((unsigned int) position.GetX() >= 0 && (unsigned int) position.GetX() < vinfo_.xres &&
      (unsigned int) position.GetY() >= 0 && (unsigned int) position.GetY() < vinfo_.yres) {
    address_offset = position.GetX() * (vinfo_.bits_per_pixel/8) + position.GetY() * finfo_.line_length;
 	  buffer_[address_offset] = color.GetB();
 	  buffer_[address_offset + 1] = color.GetG();
 	  buffer_[address_offset + 2] = color.GetR();
 	  buffer_[address_offset + 3] = 0;
  }
}

/* Draw a line with specified color from the specified start and end point
in the framebuffer */
void Framebuffer::DrawLine(const Point& start, const Point& end, const Color& color) {
	if (start.GetX() == end.GetX()) {
		int x = start.GetX();
		if (start.GetY() < end.GetY()) {
			for (int y = start.GetY(); y <= end.GetY(); y++) {
				SetPixel(Point(x, y), color);
			}
		} else {
			for (int y = end.GetY(); y <= start.GetY(); y++) {
				SetPixel(Point(x, y), color);
			}
		}
	} else if (start.GetY() == end.GetY()) {
		int y = start.GetY();
		if (start.GetX() < end.GetX()) {
			for (int x = start.GetX(); x <= end.GetX(); x++) {
				SetPixel(Point(x, y), color);
			}
		} else {
			for (int x = end.GetX(); x <= start.GetX(); x++) {
				SetPixel(Point(x, y), color);
			}
		}
	} else if (abs(end.GetY() - start.GetY()) < abs(end.GetX() - start.GetX())) {
		if (start.GetX() > end.GetX()) {
			DrawLineLow(end, start, color);
		} else {
			DrawLineLow(start, end, color);
		}
	} else {
		if (start.GetY() > end.GetY()) {
			DrawLineHigh(end, start, color);
		} else {
			DrawLineHigh(start, end, color);
		}
	}
}


/* Draw a line with specified color from the specified start and end point
with low gradient (0 < m < 1 or -1 < m < 0) in the framebuffer using Bresenham algorithm */
void Framebuffer::DrawLineLow(const Point& start, const Point& end, const Color& color) {
	int dx = end.GetX() - start.GetX();
	int dy = end.GetY() - start.GetY();

	int yi;
	if (dy < 0) {
		yi = -1;
		dy = -dy;
	} else {
		yi = 1;
	}

	int p = 2 * dy - dx;
	int y = start.GetY();

	for (int x = start.GetX(); x <= end.GetX(); x++) {
		SetPixel(Point(x, y), color);
		if (p > 0) {
			y += yi;
			p -= (2 * dx);
		}
		p += (2 * dy);
	}
}

/* Draw a line with specified color from the specified start and end point
with steep gradient (> 1 or < -1) in the framebuffer using Bresenham algorithm */
void Framebuffer::DrawLineHigh(const Point& start, const Point& end, const Color& color) {
	int dx = end.GetX() - start.GetX();
	int dy = end.GetY() - start.GetY();

	int xi;
	if (dx < 0) {
		xi = -1;
		dx = -dx;
	} else {
		xi = 1;
	}
	int p = 2 * dx - dy;
	int x = start.GetX();

	for (int y = start.GetY(); y <= end.GetY(); y++) {
		SetPixel(Point(x, y), color);
		if (p > 0) {
			x += xi;
			p -= (2 * dy);
		}
		p += (2 * dx);
	}
}

/* Draw a dotted line with specified color from the specified start and end point
in the framebuffer */
void Framebuffer::DrawDottedLine(const Point& start, const Point& end, const Color& color, int interval) {
	bool draw = false;

	if (start.GetX() == end.GetX()) {
		int x = start.GetX();
		if (start.GetY() < end.GetY()) {
			for (int y = start.GetY(); y <= end.GetY(); y++) {
				if ((y - start.GetY()) % interval == 0) {
					draw = !draw;
				}
				if (draw) {
					SetPixel(Point(x, y), color);
				}
			}
		} else {
			for (int y = end.GetY(); y <= start.GetY(); y++) {
				if ((y - end.GetY()) % interval == 0) {
					draw = !draw;
				}
				if (draw) {
					SetPixel(Point(x, y), color);
				}
			}
		}
	} else if (start.GetY() == end.GetY()) {
		int y = start.GetY();
		if (start.GetX() < end.GetX()) {
			for (int x = start.GetX(); x <= end.GetX(); x++) {
				if ((x - start.GetX()) % interval == 0) {
					draw = !draw;
				}
				if (draw) {
					SetPixel(Point(x, y), color);
				}
			}
		} else {
			for (int x = end.GetX(); x <= start.GetX(); x++) {
				if ((x - end.GetX()) % interval == 0) {
					draw = !draw;
				}
				if (draw) {
					SetPixel(Point(x, y), color);
				}
			}
		}
	} else if (abs(end.GetY() - start.GetY()) < abs(end.GetX() - start.GetX())) {
		if (start.GetX() > end.GetX()) {
			DrawDottedLineLow(end, start, color, interval);
		} else {
			DrawDottedLineLow(start, end, color, interval);
		}
	} else {
		if (start.GetY() > end.GetY()) {
			DrawDottedLineHigh(end, start, color, interval);
		} else {
			DrawDottedLineHigh(start, end, color, interval);
		}
	}
}

/* Draw a dotted line with specified color from the specified start and end point
with low gradient (0 < m < 1 or -1 < m < 0) in the framebuffer using Bresenham algorithm */
void Framebuffer::DrawDottedLineLow(const Point& start, const Point& end, const Color& color, int interval) {
	int dx = end.GetX() - start.GetX();
	int dy = end.GetY() - start.GetY();

	int yi;
	if (dy < 0) {
		yi = -1;
		dy = -dy;
	} else {
		yi = 1;
	}

	int p = 2 * dy - dx;
	int y = start.GetY();

	bool draw = false;
	for (int x = start.GetX(); x <= end.GetX(); x++) {
		if ((x - start.GetX()) % interval == 0) {
			draw = !draw;
		}
		if (draw) {
			SetPixel(Point(x, y), color);
		}
		if (p > 0) {
			y += yi;
			p -= (2 * dx);
		}
		p += (2 * dy);
	}
}

/* Draw a dotted line with specified color from the specified start and end point
with steep gradient (> 1 or < -1) in the framebuffer using Bresenham algorithm */
void Framebuffer::DrawDottedLineHigh(const Point& start, const Point& end, const Color& color, int interval) {
	int dx = end.GetX() - start.GetX();
	int dy = end.GetY() - start.GetY();

	int xi;
	if (dx < 0) {
		xi = -1;
		dx = -dx;
	} else {
		xi = 1;
	}
	int p = 2 * dx - dy;
	int x = start.GetX();

	bool draw = false;
	for (int y = start.GetY(); y <= end.GetY(); y++) {
		if ((y - start.GetY()) % interval == 0) {
			draw = !draw;
		}
		if (draw) {
			SetPixel(Point(x, y), color);
		}
		if (p > 0) {
			x += xi;
			p -= (2 * dy);
		}
		p += (2 * dx);
	}
}

/* Draw a polygon to the framebuffer */
void Framebuffer::DrawPolygon(const Polygon& polygon, const Color& color) {
	for (int i = 0; i < polygon.GetNumOfPoints(); i++) {
				DrawLine(polygon.GetPoint(i), polygon.GetPoint((i + 1) % polygon.GetNumOfPoints()), color);
	}
}

/* Draw a rastered polygon to the framebuffer */
void Framebuffer::DrawRasteredPolygon(const Polygon& polygon, const Color& border_color, const Color& fill_color,  const Point& top_left, const Point& bottom_right, int xoffset, int yoffset) {
		int ymin = 1000000;
		int ymax = 0;
		for (int i = 0; i < polygon.GetNumOfPoints(); i++) {
			if (polygon.GetPoint(i).GetY() + yoffset < ymin) {
				ymin = polygon.GetPoint(i).GetY() + yoffset;
			}
			if (polygon.GetPoint(i).GetY() + yoffset > ymax) {
				ymax = polygon.GetPoint(i).GetY() + yoffset;
			}
		}

		std::vector<std::vector<int>> intersections(ymax - ymin + 1);

		int prev, next;
		for (int i = 0; i < polygon.GetNumOfPoints(); i++) {
	    prev = i - 1;
	    next = i + 1;
	    if (prev < 0) {
	      prev = polygon.GetNumOfPoints() - 1;
	    }
			if (next == polygon.GetNumOfPoints()) {
	      next = 0;
	    }
			SetRasteredPolygonIntersections(polygon.GetPoint(i).Translate(Point(xoffset, yoffset)), polygon.GetPoint(next).Translate(Point(xoffset, yoffset)), intersections, ymin);

			/* Corner points case */
	    if ((polygon.GetPoint(i).GetY() > polygon.GetPoint(next).GetY() && polygon.GetPoint(i).GetY() < polygon.GetPoint(prev).GetY()) ||
	        (polygon.GetPoint(i).GetY() < polygon.GetPoint(next).GetY() && polygon.GetPoint(i).GetY() > polygon.GetPoint(prev).GetY())) {
	          intersections[polygon.GetPoint(i).GetY() + yoffset - ymin].push_back(polygon.GetPoint(i).GetX() + xoffset);
	    }
	    if (polygon.GetPoint(i).GetY() == polygon.GetPoint(next).GetY()) {
	      if (polygon.GetPoint(i).GetY() < polygon.GetPoint(prev).GetY()) {
	        intersections[polygon.GetPoint(i).GetY() + yoffset - ymin].push_back(polygon.GetPoint(i).GetX() + xoffset);
	      }
	      if (polygon.GetPoint(next).GetY() < polygon.GetPoint((next + 1) % polygon.GetNumOfPoints()).GetY()) {
	        intersections[polygon.GetPoint(next).GetY() + yoffset - ymin].push_back(polygon.GetPoint(next).GetX() + xoffset);
	      }
	    }
	  }

		/* Sort intersection points */
		for (int i = 1; i < (ymax - ymin); i++) {
			std::sort(intersections[i].begin(), intersections[i].end());
		}

		/* Fill polygon */
	  for (int i = 1; i < (ymax - ymin); i++) {
			if (intersections[i].size() > 1) {
				for (unsigned int j = 0; j < intersections[i].size() - 1; j += 2) {
						ClipLine(Point(intersections[i][j] + 1, ymin + i), Point(intersections[i][j + 1] - 1, ymin + i), top_left, bottom_right, fill_color);
				}
			}
		}

		/* Draw polygon outlines */
		for (int i = 0; i < polygon.GetNumOfPoints(); i++) {
			next = i + 1;
			if (next == polygon.GetNumOfPoints()) {
				next = 0;
			}
			ClipLine(polygon.GetPoint(i).Translate(Point(xoffset, yoffset)), polygon.GetPoint(next).Translate(Point(xoffset, yoffset)), top_left, bottom_right, border_color);
	}
}


/* Set the rastered polygon line intersections */
void Framebuffer::SetRasteredPolygonIntersections(const Point& start, const Point& end, std::vector<std::vector<int>>& intersections, int ymin) {
	if (start.GetX() == end.GetX()) {
		int x = start.GetX();
		if (start.GetY() < end.GetY()) {
			for (int y = start.GetY(); y <= end.GetY(); y++) {
				intersections[y - ymin].push_back(x);
			}
		} else {
			for (int y = end.GetY(); y <= start.GetY(); y++) {
				intersections[y - ymin].push_back(x);
			}
		}
	} else if (start.GetY() == end.GetY()) {
		int y = start.GetY();
		if (start.GetX() < end.GetX()) {
				intersections[y - ymin].push_back(start.GetX());
				intersections[y - ymin].push_back(end.GetX());
		} else {
			intersections[y - ymin].push_back(end.GetX());
			intersections[y - ymin].push_back(start.GetX());
		}
	} else if (abs(end.GetY() - start.GetY()) < abs(end.GetX() - start.GetX())) {
		if (start.GetX() > end.GetX()) {
			SetRasteredPolygonIntersectionsLow(end, start, intersections, ymin);
		} else {
			SetRasteredPolygonIntersectionsLow(start, end, intersections, ymin);
		}
	} else {
		if (start.GetY() > end.GetY()) {
			SetRasteredPolygonIntersectionsHigh(end, start, intersections, ymin);
		} else {
			SetRasteredPolygonIntersectionsHigh(start, end, intersections, ymin);
		}
	}
}

/* Set the rastered polygon line with low gradient (0 < m < 1 or -1 < m < 0)
intersections */
void Framebuffer::SetRasteredPolygonIntersectionsLow(const Point& start, const Point& end, std::vector<std::vector<int>>& intersections, int ymin) {
	int dx = end.GetX() - start.GetX();
	int dy = end.GetY() - start.GetY();

	int yi;
	if (dy < 0) {
		yi = -1;
		dy = -dy;
	} else {
		yi = 1;
	}

	int p = 2 * dy - dx;
	int y = start.GetY();

	for (int x = start.GetX(); x <= end.GetX(); x++) {
		if (p > 0) {
			intersections[y - ymin].push_back(x);
			y += yi;
			p -= (2 * dx);
		}
		p += (2 * dy);
	}
}

/* Set the rastered polygon line with low gradient (0 < m < 1 or -1 < m < 0)
intersections */
void Framebuffer::SetRasteredPolygonIntersectionsHigh(const Point& start, const Point& end, std::vector<std::vector<int>>& intersections, int ymin) {
	int dx = end.GetX() - start.GetX();
	int dy = end.GetY() - start.GetY();

	int xi;
	if (dx < 0) {
		xi = -1;
		dx = -dx;
	} else {
		xi = 1;
	}
	int p = 2 * dx - dy;
	int x = start.GetX();

	for (int y = start.GetY(); y <= end.GetY(); y++) {
		intersections[y - ymin].push_back(x);
		if (p > 0) {
			x += xi;
			p -= (2 * dy);
		}
		p += (2 * dx);
	}
}

/* Display the framebuffer */
void Framebuffer::Display() {
  memcpy(address_, buffer_, screen_memory_size_);
}

/* Clear the framebuffer (Set all pixel to black )*/
void Framebuffer::Clear() {
  for (int y = 0; y < GetHeight(); y++) {
    for (int x = 0; x < GetWidth(); x++) {
      SetPixel(Point(x, y), COLOR_BLACK);
    }
  }
}

/* Getter */
long Framebuffer::GetHeight() const {
	return vinfo_.yres;
}

long Framebuffer::GetWidth() const {
	return vinfo_.xres;
}

Color Framebuffer::GetPixelColor(const Point& position) const {
	Color color(0, 0, 0);
	if (position.GetX() >= 0 && (unsigned int) position.GetX() < vinfo_.xres && position.GetY() >= 0 && (unsigned int) position.GetY() < vinfo_.yres) {
		long address_offset = position.GetX() * (vinfo_.bits_per_pixel/8) + position.GetY() * finfo_.line_length;
		color.SetB(buffer_[address_offset]);
		color.SetG(buffer_[address_offset + 1]);
		color.SetR(buffer_[address_offset + 2]);
	}
	return color;
}

/* Compute the bit code for a point (x, y) using the clip rectangle */
int Framebuffer::ComputeOutCode(const Point& p, const Point& top_left, const Point& bottom_right) {
	int code = INSIDE;

	if (p.GetX() < top_left.GetX()) {
		code |= LEFT;
	} else if (p.GetX() > bottom_right.GetX()) {
		code |= RIGHT;
	}
	if (p.GetY() < top_left.GetY()) {
		code |= TOP;
	} else if (p.GetY() > bottom_right.GetY()) {
		code |= BOTTOM;
	}

	return code;
}

/* Cohen–Sutherland clipping algorithm clips a line from p1 = (x1, y1) to p2 = (x2, y2) against a rectangle */
void Framebuffer::ClipLine(const Point& p1, const Point& p2, const Point& top_left, const Point& bottom_right, Color color) {
	int outcode1 = ComputeOutCode(p1, top_left, bottom_right);
	int outcode2 = ComputeOutCode(p2, top_left, bottom_right);

	bool accept = false;
	Point clipped_p1 = p1;
	Point clipped_p2 = p2;
	Point p;
	int outcode_out;

	while(true) {
		if (!(outcode1 | outcode2)) {
			accept = 1;
			break;
		} else if (outcode1 & outcode2) {
			break;
		} else {
			outcode_out = outcode1 ? outcode1 : outcode2;

			if (outcode_out & BOTTOM) {
				p.SetX(clipped_p1.GetX() + (clipped_p2.GetX() - clipped_p1.GetX()) * (bottom_right.GetY() - clipped_p1.GetY()) / (clipped_p2.GetY() - clipped_p1.GetY()));
				p.SetY(bottom_right.GetY());
			} else if (outcode_out & TOP) {
				p.SetX(clipped_p1.GetX() + (clipped_p2.GetX() - clipped_p1.GetX()) * (top_left.GetY() - clipped_p1.GetY()) / (clipped_p2.GetY() - clipped_p1.GetY()));
				p.SetY(top_left.GetY());
			} else if (outcode_out & RIGHT) {
				p.SetY(clipped_p1.GetY() + (clipped_p2.GetY() - clipped_p1.GetY()) * (bottom_right.GetX() - clipped_p1.GetX()) / (clipped_p2.GetX() - clipped_p1.GetX()));
				p.SetX(bottom_right.GetX());
			} else if (outcode_out & LEFT) {
				p.SetY(clipped_p1.GetY() + (clipped_p2.GetY() - clipped_p1.GetY()) * (top_left.GetX() - clipped_p1.GetX()) / (clipped_p2.GetX() - clipped_p1.GetX()));
				p.SetX(top_left.GetX());
			}

			if (outcode_out == outcode1) {
				clipped_p1 = p;
				outcode1 = ComputeOutCode(clipped_p1, top_left, bottom_right);
			} else {
				clipped_p2 = p;
				outcode2 = ComputeOutCode(clipped_p2, top_left, bottom_right);
			}
		}
	}

	if (accept) {
		DrawLine(clipped_p1, clipped_p2, color);
	}
}

#include <iostream>

/* Draw a circle with specified color from the specified center and radius in the framebuffer using midpoint circle algorithm */
void Framebuffer::DrawCircle(const Point& center, int radius, const Color& color) {
    // When radius is zero only a single
    // point will be printed
    if (radius > 0)
    {
			int x = radius, y = 0;

			SetPixel(Point(x + center.GetX(), y + center.GetY()), color);
			SetPixel(Point(-x + center.GetX(), y + center.GetY()), color);
			SetPixel(Point(y + center.GetX(), x + center.GetY()), color);
			SetPixel(Point(y + center.GetX(), -x + center.GetY()), color);

			// Initialising the value of P
			int P = 1 - radius;
			while (x > y) {
				y++;

				// Mid-point is inside or on the perimeter
				if (P <= 0) {
					P = P + 2 * y + 1;
				} else { // Mid-point is outside the perimeter
					x--;
					P = P + 2*y - 2*x + 1;
				}

				// All the perimeter points have already been printed
				if (x < y) {
					break;
				}

				// Printing the generated point and its reflection
				// in the other octants after translation
				SetPixel(Point(x + center.GetX(), y + center.GetY()), color);
				SetPixel(Point(-x + center.GetX(), y + center.GetY()), color);
				SetPixel(Point(x + center.GetX(), -y + center.GetY()), color);
				SetPixel(Point(-x + center.GetX(), -y + center.GetY()), color);

				// If the generated point is on the line x = y then
				// the perimeter points have already been printed
				if (x != y) {
					SetPixel(Point(y + center.GetX(), x + center.GetY()), color);
					SetPixel(Point(-y + center.GetX(), x + center.GetY()), color);
					SetPixel(Point(y + center.GetX(), -x + center.GetY()), color);
					SetPixel(Point(-y + center.GetX(), -x + center.GetY()), color);
				}
			}
    } else {
			SetPixel(center, color);
		}
}


  /* Draw a filled circle with specified color from the specified center and radius in the framebuffer using midpoint circle algorithm */
  void Framebuffer::DrawFilledCircle(const Point& center, int radius, const Color& border_color, const Color& fill_color) {
		// When radius is zero only a single
		// point will be printed
		if (radius > 0)
		{
			int x = radius, y = 0;

			DrawLine(Point(x + center.GetX(), y + center.GetY()), Point(-x + center.GetX(), y + center.GetY()), fill_color);
			DrawLine(Point(y + center.GetX(), x + center.GetY()), Point(y + center.GetX(), -x + center.GetY()), fill_color);

			// Initialising the value of P
			int P = 1 - radius;
			while (x > y) {
				y++;

				// Mid-point is inside or on the perimeter
				if (P <= 0) {
					P = P + 2 * y + 1;
				} else { // Mid-point is outside the perimeter
					x--;
					P = P + 2*y - 2*x + 1;
				}

				// All the perimeter points have already been printed
				if (x < y) {
					break;
				}

				// Printing the generated point and its reflection
				// in the other octants after translation
				DrawLine(Point(x + center.GetX(), y + center.GetY()), Point(-x + center.GetX(), y + center.GetY()), fill_color);
				DrawLine(Point(x + center.GetX(), -y + center.GetY()), Point(-x + center.GetX(), -y + center.GetY()), fill_color);

				// If the generated point is on the line x = y then
				// the perimeter points have already been printed
				if (x != y) {
					DrawLine(Point(y + center.GetX(), x + center.GetY()), Point(-y + center.GetX(), x + center.GetY()), fill_color);
					DrawLine(Point(y + center.GetX(), -x + center.GetY()), Point(-y + center.GetX(), -x + center.GetY()), fill_color);
				}
			}
		} else {
			SetPixel(center, fill_color);
		}
		DrawCircle(center, radius, border_color);
	}

/* Draw a sprite to the framebuffer */
void Framebuffer::DrawSprite(const Sprite& sprite, int xoffset, int yoffset) {
	for (unsigned i = 0; i < sprite.polygons_.size(); i++) {
		DrawRasteredPolygon(sprite.polygons_[i], sprite.border_colors_[i], sprite.fill_colors_[i], Point(0, 0), Point(GetWidth() - 1, GetHeight() - 1), xoffset, yoffset);
	}
}

/* Draw a sprite (clipped) to the framebuffer */
void Framebuffer::DrawClippedSprite(const Sprite& sprite, const Point& top_left, const Point& bottom_right, int xoffset, int yoffset) {
	for (unsigned i = 0; i < sprite.polygons_.size(); i++) {
		DrawRasteredPolygon(sprite.polygons_[i], sprite.border_colors_[i], sprite.fill_colors_[i], top_left, bottom_right, xoffset, yoffset);
	}
}
