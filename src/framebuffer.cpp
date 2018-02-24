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
void Framebuffer::DrawRasteredPolygon(const Polygon& polygon, const Color& border_color, const Color& fill_color, int xoffset, int yoffset) {
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
					for (int k = intersections[i][j] + 1; k < intersections[i][j + 1]; k++) {
						SetPixel(Point(k, ymin + i), fill_color);
					}
				}
			}
		}

		/* Draw polygon outlines */
		for (int i = 0; i < polygon.GetNumOfPoints(); i++) {
			next = i + 1;
			if (next == polygon.GetNumOfPoints()) {
				next = 0;
			}
			DrawLine(polygon.GetPoint(i).Translate(Point(xoffset, yoffset)), polygon.GetPoint(next).Translate(Point(xoffset, yoffset)), border_color);
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
