#include "framebuffer.h"
#include "input.h"
#include "polygon.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <unistd.h>
using namespace std;

typedef pair<Point, Point> Line;

#define FPS 60
#define FONT_HEIGHT 61
#define FONT_WIDTH 43
#define VERTICAL_SPACE 15
#define HORIZONTAL_SPACE 10
#define SCALE 3
#define ROCKET_SCALE 25

struct Rocket {
  vector<Line> lines;
  int x_speed;
  int y_speed;
  Point top_left;
  Point bottom_right;
  bool destroyed;
};

struct Gunfire {
  Line line;
  int x_speed;
  int y_speed;
};

Gunfire CreateGunfire(int type, const Framebuffer& fb) {
  Gunfire gunfire;
  switch (type) {
    case 0:
      gunfire.line = make_pair(Point(fb.GetWidth() / 2 - 1, fb.GetHeight() - 1), Point(fb.GetWidth() / 2 + 14, fb.GetHeight() - 21));
      gunfire.x_speed = 25;
      gunfire.y_speed = -30;
      break;
    case 1:
      gunfire.line = make_pair(Point(fb.GetWidth() / 2 - 1, fb.GetHeight() - 1), Point(fb.GetWidth() / 2 - 1, fb.GetHeight() - 21));
      gunfire.x_speed = 0;
      gunfire.y_speed = -30;
      break;
    case 2:
      gunfire.line = make_pair(Point(fb.GetWidth() / 2 - 1, fb.GetHeight() - 1), Point(fb.GetWidth() / 2 - 16, fb.GetHeight() - 21));
      gunfire.x_speed = -25;
      gunfire.y_speed = -30;
      break;
  }
  return gunfire;
}

int main() {
  Framebuffer fb("/dev/fb0");

  /* Load font */
  vector<Polygon> alphabets[26];
  ifstream font_file;
  font_file.open("../data/font.txt");
  if (font_file.is_open()) {
    char character;
    int polygon_count;
    for (int i = 0; i < 26; i++) {
      font_file >> character >> polygon_count;
      for (int j = 0; j < polygon_count; j++) {
        int point_count;
        font_file >> point_count;
        int x, y;
        Polygon temp;
        for (int k = 0; k < point_count; k++) {
          font_file >> x >> y;
           x *= SCALE;
           y *= SCALE;
          temp.AddPoint(Point(x, y));
        }
        alphabets[character - 'A'].push_back(temp);
      }
    }
    font_file.close();
  }

  /* Load rocket */
  Rocket rocket;
  ifstream rocket_file;
  rocket_file.open("../data/rocket.txt");
  if (rocket_file.is_open()) {
    int line_count;
    rocket_file >> line_count;
    int x1, y1, x2, y2;
    for (int i = 0; i < line_count; i++) {
      rocket_file >> x1 >> y1 >> x2 >> y2;
      rocket.lines.push_back(make_pair(Point(x1, y1), Point(x2, y2)));
      rocket.lines[i].first.Scale(Point(0, 0), ROCKET_SCALE).Translate(Point(fb.GetWidth(), 0));
      rocket  .lines[i].second.Scale(Point(0, 0), ROCKET_SCALE).Translate(Point(fb.GetWidth(), 0));
    }
    rocket_file >> x1 >> y1 >> x2 >> y2;
    rocket.top_left = Point::Scale(Point(x1, y1), Point(0, 0), ROCKET_SCALE).Translate(Point(fb.GetWidth(), 0));
    rocket.bottom_right = Point::Scale(Point(x2, y2), Point(0, 0), ROCKET_SCALE).Translate(Point(fb.GetWidth(), 0));
    rocket.x_speed = -9;
    rocket.y_speed = 0;
    rocket.destroyed = false;
    rocket_file.close();
  }

  string names[9] = {
    "JEMJEM",
    "",
    "ADITYA PRATAMA",
    "JORDHY FERNANDO",
    "MUHAMMAD FARHAN KEMAL",
    "SYLVIA JULIANA",
    "TASYA",
    "TURFA AULIARACHMAN",
    "VIGOR AKBAR"
  };

  Color names_color[9] = {
    COLOR_BRONZE,
    COLOR_BLACK,
    COLOR_RED,
    COLOR_ORANGE,
    COLOR_YELLOW,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_INDIGO,
    COLOR_VIOLET
  };

  vector<Gunfire> gunfires;

  /* Counter to decide gunfire type to be added */
  int counter = -1;

  Input input;

  /* Starting ordinat for names */
  int start = fb.GetHeight();

  int millis_per_frame = 1000 / FPS;

  /* Main loop */
  while (start >= 0) {
    fb.Clear();

    /* Draw names */
    for (int line = 0; line < 9; line++) {
      int len = names[line].length();
      for (int i = 0; i < len; i++) {
        int yoffset = start + line * (FONT_HEIGHT + VERTICAL_SPACE);
        int xoffset = i * (FONT_WIDTH + HORIZONTAL_SPACE) + ((fb.GetWidth() - (len * (FONT_WIDTH + HORIZONTAL_SPACE) - HORIZONTAL_SPACE)) / 2);
        if (names[line][i] != ' ') {
          int idx = names[line][i] - 'A';
          for (unsigned int j = 0; j < alphabets[idx].size(); j++) {
            if (j == 0) {
              fb.DrawRasteredPolygon(alphabets[idx][j], COLOR_WHITE, names_color[line], xoffset, yoffset);
            } else {
                fb.DrawRasteredPolygon(alphabets[idx][j], COLOR_WHITE, COLOR_BLACK, xoffset, yoffset);
            }
          }
        }
      }
    }
    start -= 5;

    /* Check user's input if space then add gunfire */
    if (input.IsKeyPressed(' ')) {
       counter = (counter + 1) % 3;
       gunfires.push_back(CreateGunfire(counter, fb));
    }
    input.Flush();

    /* Check collision (only need to check first 3 gunfires) */
    unsigned int i = 0;
    while (!rocket.destroyed && i < 3 && i < gunfires.size()) {
        if ((rocket.top_left.GetX() <= gunfires[i].line.first.GetX() && gunfires[i].line.first.GetX() <= rocket.bottom_right.GetX() &&
            rocket.top_left.GetY() <= gunfires[i].line.first.GetY() && gunfires[i].line.first.GetY() <= rocket.bottom_right.GetY()) ||
            (rocket.top_left.GetX() <= gunfires[i].line.second.GetX() && gunfires[i].line.second.GetX() <= rocket.bottom_right.GetX() &&
            rocket.top_left.GetY() <= gunfires[i].line.second.GetY() && gunfires[i].line.second.GetY() <= rocket.bottom_right.GetY())) {
            rocket.destroyed = true;
            gunfires[i].line.first.Translate(Point(0, -1000));
            gunfires[i].line.second.Translate(Point(0, -1000));
        }
      i++;
    }

    /* Draw rocket */
    if (!rocket.destroyed) {
      for (unsigned int i = 0; i < rocket.lines.size(); i++) {
        rocket.lines[i].first.Translate(Point(rocket.x_speed, 0));
        rocket.lines[i].second.Translate(Point(rocket.x_speed, 0));
        fb.DrawLine(rocket.lines[i].first, rocket.lines[i].second, COLOR_CYAN);
      }
      rocket.top_left.Translate(Point(rocket.x_speed, 0));
      rocket.bottom_right.Translate(Point(rocket.x_speed, 0));
    }

    /* Draw gunfires */
    for (vector<Gunfire>::iterator it = gunfires.begin(); it != gunfires.end();) {
      if (it->line.first.GetY() < 0) {
        it = gunfires.erase(it);
      } else {
        fb.DrawLine(it->line.first, it->line.second, COLOR_SILVER);
        it->line.first.Translate(Point(it->x_speed, it->y_speed));
        it->line.second.Translate(Point(it->x_speed, it->y_speed));
        ++it;
      }
    }

    fb.Display();
    usleep(millis_per_frame * 1000);
  }

  input.Flush();
  return 0;
}
