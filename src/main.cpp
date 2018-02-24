#include "framebuffer.h"
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
using namespace std;

#define FPS 60
#define FONT_HEIGHT 56
#define FONT_WIDTH 40
#define VERTICAL_SPACE 15
#define HORIZONTAL_SPACE 7

int main()
{
  Framebuffer fb("/dev/fb0");
  string alphabets[26][FONT_HEIGHT];
  string empty_line;
  int counter = 0;

  /* Load font */
  ifstream font_file;
  font_file.open("../data/font.txt");
  if (font_file.is_open()) {
    for (int i = 0; i < 26; i++) {
      for (int j = 0; j < FONT_HEIGHT; j++) {
        getline(font_file, alphabets[counter][j]);
      }
      getline(font_file, empty_line);
      counter++;
    }
    font_file.close();
  }

  string names[7] = {
    "ADITYA PRATAMA",
    "JORDHY FERNANDO",
    "MUHAMMAD FARHAN KEMAL",
    "SYLVIA JULIANA",
    "TASYA",
    "TURFA AULIARACHMAN",
    "VIGOR AKBAR"
  };

  Color names_color[7] = {
    COLOR_RED,
    COLOR_ORANGE,
    COLOR_YELLOW,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_INDIGO,
    COLOR_VIOLET
  };

  int start = fb.GetHeight();
  int millis_per_frame = 1000 / FPS;

  while (start >= 0) {
    fb.Clear();
    for (int line = 0; line < 7; line++) {
      int len = names[line].length();
      for (int i = 0; i < len; i++) {
        int yoffset = line * (FONT_HEIGHT + VERTICAL_SPACE);
        for (int y = start + yoffset; y < start + yoffset + FONT_HEIGHT; y++) {
          int xoffset = i * (FONT_WIDTH + HORIZONTAL_SPACE) + ((fb.GetWidth() - (len * (FONT_WIDTH + HORIZONTAL_SPACE) - HORIZONTAL_SPACE)) / 2);
          for (int x = xoffset; x < xoffset + FONT_WIDTH; x++) {
            if (names[line][i] != ' ' && alphabets[names[line][i] - 'A'][y - (start + yoffset)][x - xoffset] == '1') {
              fb.SetPixel(Point(x, y), names_color[line]);
            }
          }
        }
      }
    }
    start -= 5;
    fb.Display();
    usleep(millis_per_frame * 1000);
  }
  return 0;
}
