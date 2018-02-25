#include "graphics/framebuffer.h"
#include "utils/input.h"
#include "objects/plane.h"
#include "objects/cannonball.h"
#include "objects/pilot.h"
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
using namespace std;

#define FPS 60

void DrawBackground(Framebuffer& fb, const Point& top_left, const Point& bottom_right) {
  for (int y = top_left.GetY(); y <= bottom_right.GetY(); y++) {
    for (int x = top_left.GetX(); x <= bottom_right.GetX(); x++) {
      if ((y == (top_left.GetY() + (bottom_right.GetY() - top_left.GetY())) / 2 + 120 || y == (top_left.GetY() + (bottom_right.GetY() - top_left.GetY())) / 2 + 121) && x % 2 == 0) {
        fb.SetPixel(Point(x, y), COLOR_DARK_GREEN);
      } else if (y > (top_left.GetY() + (bottom_right.GetY() - top_left.GetY())) / 2 + 121) {
        fb.SetPixel(Point(x, y), COLOR_GREEN);
      } else {
        fb.SetPixel(Point(x, y), COLOR_DEEP_SKY_BLUE);
      }
    }
  }
}

CannonBall CreateCannonBall(int type, const Point& top_left, const Point& bottom_right) {
  if (type == 0) {
    CannonBall cannon_ball("../data/cannonball.txt", 5, -60);
    cannon_ball.SetCenter(Point(top_left.GetX() + (bottom_right.GetX() - top_left.GetX()) / 2, top_left.GetY() + (bottom_right.GetY() - top_left.GetY())));
    return cannon_ball;

  } else if (type == 1) {
    CannonBall cannon_ball("../data/cannonball.txt", 0, -60);
    cannon_ball.SetCenter(Point(top_left.GetX() + (bottom_right.GetX() - top_left.GetX()) / 2, top_left.GetY() + (bottom_right.GetY() - top_left.GetY())));
    return cannon_ball;
  } else {
    CannonBall cannon_ball("../data/cannonball.txt", -5, -60);
    cannon_ball.SetCenter(Point(top_left.GetX() + (bottom_right.GetX() - top_left.GetX()) / 2, top_left.GetY() + (bottom_right.GetY() - top_left.GetY())));
    return cannon_ball;
  }
}

int main() {
  Framebuffer fb("/dev/fb0");

  /* Initialize view */
  Point top_left(150, 150);
  Point bottom_right(fb.GetWidth() -150, fb.GetHeight() - 150);

  Plane plane("../data/plane.txt");
  plane.SetCenter(Point(top_left.GetX() + (bottom_right.GetX() - top_left.GetX()) / 2, top_left.GetY() + (bottom_right.GetY() - top_left.GetY()) / 2 - 100));

  Pilot pilot("../data/pilot.txt", -60);
  pilot.SetCenter(Point(top_left.GetX() + (bottom_right.GetX() - top_left.GetX()) / 2, top_left.GetY() + (bottom_right.GetY() - top_left.GetY()) / 2 - 100));

  Input input;
  int millis_per_frame = 1000 / FPS;
  vector<CannonBall> cannon_balls;

  int counter = -1;
  srand(time(NULL));

  /* Main loop */
  while (true) {
    DrawBackground(fb, top_left, bottom_right);

    /* Read user's input */
    char key = input.GetKeyPressed();
    if (key == ' ') {
      counter = (counter + 1) % 3;
      cannon_balls.push_back(CreateCannonBall(counter, top_left, bottom_right));
    } else if (key == 'z') {
      plane.Scale(2);
      pilot.Scale(2);
    } else if (key == 'x') {
      plane.Scale(0.5);
      pilot.Scale(0.5);
    } else if (key == 'q') {
      break;
    }
    input.Flush();

    if (plane.IsHit()) {
      pilot.Render(fb, top_left, bottom_right);
    }

    plane.Render(fb, top_left, bottom_right);

    /* Remove destroyed cannon balls */
    for (vector<CannonBall>::iterator it = cannon_balls.begin(); it != cannon_balls.end();) {
      if (it->IsDestroyed()) {
        it = cannon_balls.erase(it);
      } else {
        ++it;
      }
    }

    /* Draw cannon balls */
    for (unsigned int j = 0; j < cannon_balls.size(); j++) {
      if (!cannon_balls[j].IsDestroyed()) {
        cannon_balls[j].Render(fb, top_left, bottom_right);
      }
    }

    /* Check cannon balls collision */
    for (unsigned int j = 0; j < cannon_balls.size(); j++) {
      int y = cannon_balls[j].y_min_;
      while (!cannon_balls[j].IsDestroyed() && y < cannon_balls[j].y_max_) {
        int x = cannon_balls[j].x_min_;
        while (!cannon_balls[j].IsDestroyed() && x < cannon_balls[j].x_max_) {
          Color color = fb.GetPixelColor(Point(x, y));
          if (Color::IsColorSame(color, COLOR_RED) || Color::IsColorSame(color, COLOR_WHITE)) {
            cannon_balls[j].SetDestroyed(true);
            plane.IncHitCounter();
          } else {
            x++;
          }
        }
        y++;
      }
    }

    plane.RotatePropellers();

    fb.Display();
    usleep(millis_per_frame * 1000);
  }
  
  input.Flush();
  fb.Clear();
  fb.Display();
  return 0;
}
