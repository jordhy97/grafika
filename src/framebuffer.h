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

  /* Draw a line with specified color from the specified start and end point
  in the framebuffer */
  void DrawLine(const Point& start, const Point& end, const Color& color);

  /* Display the framebuffer */
  void Display();

  /* Clear the framebuffer (Set all pixel to black )*/
  void Clear();

  /* Getter */
  long GetHeight() const;
  long GetWidth() const;

private:
  /* Draw a line with specified color from the specified start and end point
  with low gradient (0 < m < 1 or -1 < m < 0) in the framebuffer using Bresenham algorithm */
  void DrawLineLow(const Point& start, const Point& end, const Color& color);
  /* Draw a line with specified color from the specified start and end point
  with steep gradient (> 1 or < -1) in the framebuffer using Bresenham algorithm */
  void DrawLineHigh(const Point& start, const Point& end, const Color& color);

  int device_;
  uint8_t *address_; /* pointer to screen memory */
  uint8_t *buffer_;
  int screen_memory_size_;
  struct fb_fix_screeninfo finfo_;
  struct fb_var_screeninfo vinfo_;
};

#endif
