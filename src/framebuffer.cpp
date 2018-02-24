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
