#include "framebuffer.h"
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>

#include <iostream>
using namespace std;

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
