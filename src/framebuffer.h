#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <linux/fb.h>
#include <stdint.h>
#include "point.h"
#include "color.h"

class Framebuffer {
public:
  /* Constructor */
  Framebuffer(const char *device_path);

  /* Destructor */
  ~Framebuffer();

  /* Set a pixel with specified color to the specified point in framebuffer */
  void SetPixel(const Point& position, const Color& color);

  /* Display the framebuffer */
  void Display();

  /* Clear the framebuffer (Set all pixel to black )*/
  void Clear();

  /* Getter */
  long GetHeight() const;
  long GetWidth() const;

private:
  int device_;
  uint8_t *address_; /* pointer to screen memory */
  uint8_t *buffer_;
  int screen_memory_size_;
  struct fb_fix_screeninfo finfo_;
  struct fb_var_screeninfo vinfo_;
};

#endif
