#ifndef COLOR_H
#define COLOR_H

#define COLOR_BLACK (Color(0, 0, 0))
#define COLOR_RED (Color(255, 0, 0))
#define COLOR_ORANGE (Color(255, 127, 0))
#define COLOR_YELLOW (Color(255, 255, 0))
#define COLOR_GREEN (Color(0, 255, 0))
#define COLOR_BLUE (Color(0, 0, 255))
#define COLOR_INDIGO (Color(75, 0, 130))
#define COLOR_VIOLET (Color(148, 0, 211))
#define COLOR_WHITE (Color(255, 255, 255))
#define COLOR_CYAN (Color(0, 255, 255))
#define COLOR_SILVER (Color(192, 192, 192))

class Color {
public:
  /* Constructor */
  Color(unsigned char r, unsigned g, unsigned b) : r_(r), g_(g), b_(b) {}

  /* Getter */
  unsigned char GetR() const { return r_; }
  unsigned char GetG() const { return g_; }
  unsigned char GetB() const { return b_; }

  /* Setter */
  void SetR(unsigned char r) { r_ = r; }
  void SetG(unsigned char g) { g_ = g; }
  void SetB(unsigned char b) { b_ = b; }

private:
  unsigned char r_;
  unsigned char g_;
  unsigned char b_;
};

#endif
