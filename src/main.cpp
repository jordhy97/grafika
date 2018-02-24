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

void DrawBackground(Framebuffer& fb) {
  for (int y = 0; y < fb.GetHeight(); y++) {
    for (int x = 0; x < fb.GetWidth(); x++) {
      if ((y == fb.GetHeight() / 2 + 120 || y == (fb.GetHeight() / 2 + 121)) && x % 2 == 0) {
        fb.SetPixel(Point(x, y), COLOR_DARK_GREEN);
      } else if (y > fb.GetHeight() / 2 + 121) {
        fb.SetPixel(Point(x, y), COLOR_GREEN);
      } else {
        fb.SetPixel(Point(x, y), COLOR_DEEP_SKY_BLUE);
      }
    }
  }
}

void flash(Framebuffer& fb) {
  int x, y;
  for (y = 0; y < fb.GetHeight(); y++) {
    for (x = 0; x < fb.GetWidth(); x++) {
      fb.SetPixel(Point(x, y), COLOR_WHITE);
    }
  }
  fb.Display();
}

CannonBall CreateCannonBall(int type, const Framebuffer& fb) {
  if (type == 0) {
    CannonBall cannon_ball("../data/cannonball.txt", 5, -60);
    cannon_ball.SetCenter(Point(fb.GetWidth() / 2 - 1, fb.GetHeight() - 1));
    return cannon_ball;

  } else if (type == 1) {
    CannonBall cannon_ball("../data/cannonball.txt", 0, -60);
    cannon_ball.SetCenter(Point(fb.GetWidth() / 2 - 1, fb.GetHeight() - 1));
    return cannon_ball;
  } else {
    CannonBall cannon_ball("../data/cannonball.txt", -5, -60);
    cannon_ball.SetCenter(Point(fb.GetWidth() / 2 - 1, fb.GetHeight() - 1));
    return cannon_ball;
  }
}

int main() {
  Framebuffer fb("/dev/fb0");

  Plane plane("../data/plane.txt");
  plane.SetCenter(Point(fb.GetWidth() / 2 - 1, fb.GetHeight() / 2 - 100));

  Pilot pilot("../data/pilot.txt", -60);
  pilot.SetCenter(Point(fb.GetWidth() / 2 - 1, fb.GetHeight() / 2 - 100));

  Input input;
  int millis_per_frame = 1000 / FPS;
  vector<CannonBall> cannon_balls;
  int counter = -1;
  srand(time(NULL));

  /* Main loop */
  for (int i = 0; i < 249; i++) {
    DrawBackground(fb);

    if (plane.IsHit()) {
      pilot.Render(fb);
    }

    plane.Render(fb);

    /* Read user's input */
    if (input.IsKeyPressed(' ')) {
      counter = (counter + 1) % 3;
      cannon_balls.push_back(CreateCannonBall(counter, fb));
    }
    input.Flush();

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
        cannon_balls[j].Render(fb);
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

    if ((i + 1) % 50 == 0) {
      /* Zoom in */
      plane.Scale(2);
      if (!plane.IsHit()) {
        pilot.Scale(2);
      }
    }
    plane.RotatePropellers();

    fb.Display();
    usleep(millis_per_frame * 1000);
  }

  flash(fb);
  usleep(millis_per_frame * 1000);
  fb.Clear();
  fb.Display();
  input.Flush();
  return 0;
}
