#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <linux/fb.h>
#include <stdint.h>
#include <vector>
#include "point.h"
#include "polygon.h"
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

  /* Draw a dotted line with specified color and interval from the specified
  start and end point in the framebuffer */
  void DrawDottedLine(const Point& start, const Point& end, const Color& color, int interval);

  /* Draw a polygon to the framebuffer */
  void DrawPolygon(const Polygon& polygon, const Color& color);

  /* Draw a rastered polygon to the framebuffer */
  void DrawRasteredPolygon(const Polygon& polygon, const Color& border_color, const Color& fill_color, int xoffset, int yoffset);

  /* Display the framebuffer */
  void Display();

  /* Clear the framebuffer (Set all pixel to black )*/
  void Clear();

  /* Getter */
  long GetHeight() const;
  long GetWidth() const;
  Color GetPixelColor(const Point& position) const;

private:
  /* Draw a line with specified color from the specified start and end point
  with low gradient (0 < m < 1 or -1 < m < 0) in the framebuffer using Bresenham algorithm */
  void DrawLineLow(const Point& start, const Point& end, const Color& color);

  /* Draw a line with specified color from the specified start and end point
  with steep gradient (> 1 or < -1) in the framebuffer using Bresenham algorithm */
  void DrawLineHigh(const Point& start, const Point& end, const Color& color);

  /* Draw a dotted line with specified color from the specified start and end point
  with low gradient (0 < m < 1 or -1 < m < 0) in the framebuffer using Bresenham algorithm */
  void DrawDottedLineLow(const Point& start, const Point& end, const Color& color, int interval);

  /* Draw a dotted line with specified color from the specified start and end point
  with steep gradient (> 1 or < -1) in the framebuffer using Bresenham algorithm */
  void DrawDottedLineHigh(const Point& start, const Point& end, const Color& color, int interval);

  /* Set the rastered polygon line intersections */
  void SetRasteredPolygonIntersections(const Point& start, const Point& end, std::vector<std::vector<int>>& intersections, int ymin);

  /* Set the rastered polygon line with low gradient (0 < m < 1 or -1 < m < 0)
  intersections */
  void SetRasteredPolygonIntersectionsLow(const Point& start, const Point& end, std::vector<std::vector<int>>& intersections, int ymin);

  /* Set the rastered polygon line with low gradient (0 < m < 1 or -1 < m < 0)
  intersections */
  void SetRasteredPolygonIntersectionsHigh(const Point& start, const Point& end, std::vector<std::vector<int>>& intersections, int ymin);

  int device_;
  uint8_t *address_; /* pointer to screen memory */
  uint8_t *buffer_;
  int screen_memory_size_;
  struct fb_fix_screeninfo finfo_;
  struct fb_var_screeninfo vinfo_;
};

#endif
