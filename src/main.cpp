#include "graphics/framebuffer.h"
#include "graphics/sprite.h"
#include "objects/font.h"
#include "objects/view.h"
#include "objects/plane.h"
#include "objects/gun_fire.h"
#include "utils/input.h"
#include "utils/mouse_listener.h"
#include <vector>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
using namespace std;

#define FPS 60
#define MAIN_SCREEN_WIDTH 1200
#define MAIN_SCREEN_HEIGHT 800
#define GAME_SCREEN_WIDTH 900
#define GAME_SCREEN_HEIGHT 800
#define MINI_MAP_WIDTH 300
#define MINI_MAP_HEIGHT 300
#define MAP_WIDTH 600
#define MAP_HEIGHT 600
#define GUN_FIRE_COLOR COLOR_SILVER
#define GUN_FIRE_LENGTH 10
#define GUN_FIRE_SPEED -15
#define PLAY 1
#define CREDITS 2
#define EXIT 3

/* Global variables */
Font font("../data/font.txt");
Framebuffer fb("/dev/fb0");
Sprite facilities("../data/facilities.txt");
Sprite buildings("../data/buildings.txt");
Sprite poles("../data/poles.txt");
Point main_screen_top_left(fb.GetWidth() / 2 - MAIN_SCREEN_WIDTH / 2, fb.GetHeight() / 2 - MAIN_SCREEN_HEIGHT / 2);
Point main_screen_bottom_right(fb.GetWidth() / 2 + MAIN_SCREEN_WIDTH / 2, fb.GetHeight() / 2 + MAIN_SCREEN_HEIGHT / 2);
View main_screen(main_screen_top_left, main_screen_bottom_right, COLOR_WHITE);

/* Function/Procedure declaration */
/* Display main menu and return the chosen option id */
int MainMenu();

/* Checks whether the given position is within the border or not */
bool IsWithinBorder(const Point& top_left, const Point& bottom_right, const Point& position);

/* Start and play the game */
void PlayGame();

/* Play the credits scene */
void PlayCredits();

int main() {
  /* initialize random seed */
  srand (time(NULL));

  int code = MainMenu();
  while(code != EXIT) {
    switch(code) {
      case PLAY:
        PlayGame();
        break;
      case CREDITS:
        PlayCredits();
        break;
    }
    code = MainMenu();
  }
  fb.Clear();
  fb.Display();
  return 0;
}

/* Function/Procedure realization */
/* Display main menu and return the chosen option id */
int MainMenu() {
  /* Init */
  Polygon cursor;
  cursor.AddPoint(Point(0, 0));
  cursor.AddPoint(Point(0, 16));
  cursor.AddPoint(Point(1, 16));
  cursor.AddPoint(Point(4, 13));
  cursor.AddPoint(Point(5, 14));
  cursor.AddPoint(Point(5, 15));
  cursor.AddPoint(Point(6, 16));
  cursor.AddPoint(Point(6, 17));
  cursor.AddPoint(Point(7, 18));
  cursor.AddPoint(Point(8, 18));
  cursor.AddPoint(Point(9, 17));
  cursor.AddPoint(Point(9, 16));
  cursor.AddPoint(Point(8, 15));
  cursor.AddPoint(Point(8, 14));
  cursor.AddPoint(Point(7, 13));
  cursor.AddPoint(Point(7, 12));
  cursor.AddPoint(Point(11, 12));
  cursor.AddPoint(Point(11, 11));
  cursor.AddPoint(Point(1, 1));
  cursor.AddPoint(Point(0, 1));

  string text[4] = {"TITLE", "PLAY", "CREDITS", "EXIT"};
  int text_scale[4] = {3, 2, 2, 2};
  Point text_top_left[4] = {Point(835, 160), Point(890, 740), Point(837, 800), Point(890, 860)};
  Point text_bottom_right[4] = {Point(1080, 223), Point(1025, 782), Point(1077, 842), Point(1025, 902)};

  Point preview_screen_top_left = Point::Translate(main_screen_top_left, Point(100, 100));
  Point preview_screen_bottom_right = Point::Translate(main_screen_bottom_right, Point(-100, -220));
  View preview_screen(preview_screen_top_left, preview_screen_bottom_right, COLOR_WHITE);
  preview_screen.AddSource(&buildings);
  preview_screen.AddSource(&facilities);
  preview_screen.AddSource(&poles);

  Point preview_source_top_left = Point(250, 500);
  Point preview_source_bottom_right = Point(350, 600);

  Plane player("../data/player_plane.txt");
  Point start = Point(preview_screen_bottom_right.GetX() - (MAIN_SCREEN_WIDTH - 200) / 2, preview_screen_bottom_right.GetY() - 75);
  player.SetCenter(start);
  player.Scale(4);

  MouseListener mouse_listener(main_screen_top_left, main_screen_bottom_right, Point::Translate(text_top_left[1], Point (70, 21)));
  Point cursor_position;
  int chosen = 0;

  while(!chosen) {
    preview_screen.SetSourcePosition(preview_source_top_left, preview_source_bottom_right);

    if(mouse_listener.IsLeftClicked()) {
      cursor_position = mouse_listener.GetPosition();
      int i = 1;
      while(!chosen && i < 4) {
        if(IsWithinBorder(text_top_left[i], text_bottom_right[i], cursor_position)) {
          chosen = i;
        } else {
          i++;
        }
      }
    }
    fb.Clear();

    /* Display main screen */
    main_screen.Render(fb);
    preview_screen.Render(fb);

    /* Display buttons and text */
    for (int i = 0; i < 4; i++) {
      Font::RenderText(text[i], font, fb, text_top_left[i], COLOR_WHITE, COLOR_RED, COLOR_BLACK, text_scale[i], main_screen_top_left, main_screen_bottom_right);
    }

    player.Render(fb, preview_screen_top_left, preview_screen_bottom_right);
    cursor_position = mouse_listener.GetPosition();
    fb.DrawRasteredPolygon(cursor, COLOR_WHITE, COLOR_WHITE, main_screen_top_left, main_screen_bottom_right, cursor_position.GetX(), cursor_position.GetY());
    fb.Display();

    /* Move preview map */
    preview_source_top_left.Translate(Point(0, -2));
    preview_source_bottom_right.Translate(Point(0, -2));
    if (preview_source_top_left.GetY() <= 0) {
      preview_source_top_left.SetY(MAP_HEIGHT);
      preview_source_bottom_right.SetY(MAP_HEIGHT + preview_source_bottom_right.GetX() - preview_source_top_left.GetX());
    }

    usleep(1000 / FPS * 1000);
  }

  return chosen;
}

/* Checks whether the given position is within the border or not */
bool IsWithinBorder(const Point& top_left, const Point& bottom_right, const Point& position) {
  return (top_left.GetX() <= position.GetX() && position.GetX() <= bottom_right.GetX() &&
          top_left.GetY() <= position.GetY() && position.GetY() <= bottom_right.GetY());
}

/* Start and play the game */
void PlayGame() {
  /* Init */
  Point mini_map_top_left = Point::Translate(main_screen_bottom_right, Point(-MINI_MAP_WIDTH, -MINI_MAP_HEIGHT));
  Point mini_map_bottom_right = main_screen_bottom_right;
  View mini_map(mini_map_top_left, mini_map_bottom_right, COLOR_WHITE);

  mini_map.AddSource(&buildings);
  mini_map.AddSource(&facilities);
  mini_map.AddSource(&poles);
  mini_map.SetSourcePosition(Point(0, 0), Point(MAP_WIDTH, MAP_HEIGHT));

  Point game_screen_top_left = main_screen_top_left;
  Point game_screen_bottom_right = Point::Translate(mini_map_bottom_right, Point(-MINI_MAP_WIDTH, 0));
  View game_screen(game_screen_top_left, game_screen_bottom_right, COLOR_WHITE);
  game_screen.AddSource(&buildings);
  game_screen.AddSource(&facilities);
  game_screen.AddSource(&poles);

  Point game_source_top_left = Point(250, 500);
  Point game_source_bottom_right = Point(350, 600);

  bool died = false;
  bool end = false;
  Plane player("../data/player_plane.txt");
  Point start = Point(game_screen_bottom_right.GetX() - GAME_SCREEN_WIDTH / 2, game_screen_bottom_right.GetY() - 75);
  player.SetCenter(start);
  player.Scale(4);
  MouseListener mouse_listener(Point::Translate(game_screen_top_left, Point(50, GAME_SCREEN_HEIGHT / 2 + 50)),
                               Point::Translate(game_screen_bottom_right, Point(-50, -54)), start);

  char key;
  Input input;
  input.Flush();

  vector<Plane> enemies;
  vector<GunFire> gun_fires;
  int counter = 1;
  int wave_time = rand() % 10 + 5;

  /* Game loop */
  while(!died && !end) {
    game_screen.SetSourcePosition(game_source_top_left, game_source_bottom_right);

    key = input.GetKeyPressed();
    if (key == 'q') {
      end = true;
    } else if (key == 'a') { /* Move map left */
      game_source_top_left.Translate(Point(-1, 0));
      game_source_bottom_right.Translate(Point(-1, 0));
      for (unsigned i = 0; i < enemies.size(); i++) {
        enemies[i].SetCenter(Point(enemies[i].GetCenter().GetX() + 5, enemies[i].GetCenter().GetY()));
      }
      if (game_source_top_left.GetX() <= 0) {
        game_source_top_left.SetX(MAP_WIDTH - (game_source_bottom_right.GetY() - game_source_top_left.GetY()));
        game_source_bottom_right.SetX(MAP_WIDTH);
      }
    } else if (key == 'd') { /* Move map right */
      game_source_top_left.Translate(Point(1, 0));
      game_source_bottom_right.Translate(Point(1, 0));
      for (unsigned i = 0; i < enemies.size(); i++) {
        enemies[i].SetCenter(Point(enemies[i].GetCenter().GetX() - 5, enemies[i].GetCenter().GetY()));
      }
      if (game_source_bottom_right.GetX() >= MAP_WIDTH) {
        game_source_top_left.SetX(0);
        game_source_bottom_right.SetX(game_source_bottom_right.GetY() - game_source_top_left.GetY());
      }
    } else if (key == 'z') { /* Zoom in */
      if (game_source_top_left.GetX() - 5 >= 0 && game_source_top_left.GetY() - 5 > 0 && game_source_bottom_right.GetX() + 5 <= 600 && game_source_bottom_right.GetY() + 5 <= 600) {
        game_source_top_left.Translate(Point(-5, -5));
        game_source_bottom_right.Translate(Point(5, 5));
      }
    } else if (key == 'x') { /* Zoom out */
      if (game_source_top_left.GetX() != game_source_bottom_right.GetX()) {
        game_source_top_left.Translate(Point(5, 5));
        game_source_bottom_right.Translate(Point(-5, -5));
      }
    }
    input.Flush();

    fb.Clear();

    /* Display game screen */
    main_screen.Render(fb);
    mini_map.Render(fb);
    game_screen.Render(fb);

    /* Read mouse input */
    if (mouse_listener.IsLeftClicked()) {
      Point start = Point::Translate(mouse_listener.GetPosition(), Point(0, -60));
      Point end = Point::Translate(mouse_listener.GetPosition(), Point(0, -60 - GUN_FIRE_LENGTH));
      gun_fires.push_back(GunFire(start, end, GUN_FIRE_COLOR, GUN_FIRE_SPEED));
    }

    /* Generate enemy wave */
    counter = (counter + 1) % wave_time;
    if (counter == 0) {
      /* Generate new wave time */
      wave_time = rand() % 11 + 5;

      /* Generate y_speed and x_position */
      int y_speed = rand() % 41 + 10;
      int x_position = game_screen_top_left.GetX() + (rand() % (GAME_SCREEN_WIDTH - 37) + 37);

      Plane enemy("../data/enemy_plane.txt", y_speed);
      enemy.Scale(3);
      enemy.SetCenter(Point(x_position, game_screen_top_left.GetY() + 45));
      enemies.push_back(enemy);
    }

    /* Display gun fires */
    for (unsigned i = 0; i < gun_fires.size(); i++) {
      gun_fires[i].Render(fb, game_screen_top_left, game_screen_bottom_right);
    }

    /* Remove hit gun fires */
    for (vector<GunFire>::iterator it = gun_fires.begin(); it != gun_fires.end(); ) {
      bool hit = false;
      unsigned i = 0;
      while (!hit && i < enemies.size()) {
        Point top_left = enemies[i].GetTopLeft();
        Point bottom_right = enemies[i].GetBottomRight();
        if ((top_left.GetX() <= it->GetStart().GetX() && it->GetStart().GetX() <= bottom_right.GetX() &&
             top_left.GetY() <= it->GetStart().GetY() && it->GetStart().GetY() <= bottom_right.GetY()) ||
            (top_left.GetX() <= it->GetEnd().GetX() && it->GetEnd().GetX() <= bottom_right.GetX() &&
             top_left.GetY() <= it->GetEnd().GetY() && it->GetEnd().GetY() <= bottom_right.GetY())) {
          hit = true;
        } else {
          i++;
        }
      }
      if (hit) {
        it = gun_fires.erase(it);
      } else {
        ++it;
      }
    }

    /* Display enemies (destroy enemy if hit with gun fire) */
    for (vector<Plane>::iterator it = enemies.begin(); it != enemies.end(); ) {
      if (it->IsCollide(fb, GUN_FIRE_COLOR)) {
        it->Render(fb, game_screen_top_left, game_screen_bottom_right);
        it = enemies.erase(it);
      } else {
        it->Render(fb, game_screen_top_left, game_screen_bottom_right);
        ++it;
      }
    }

    /* Game over if collided with enemy */
    if (player.IsCollide(fb, COLOR_RED)) {
      player.SetCenter(mouse_listener.GetPosition());
      player.Render(fb, game_screen_top_left, game_screen_bottom_right);
      fb.Display();
      died = true;
    }

    player.SetCenter(mouse_listener.GetPosition());
    player.Render(fb, game_screen_top_left, game_screen_bottom_right);
    fb.Display();

    /* Move gun fires and remove out of frame gun fires */
    for (vector<GunFire>::iterator it = gun_fires.begin(); it != gun_fires.end(); ) {
      it->Move();
      if (it->GetStart().GetY() <= game_screen_top_left.GetY()) {
          it = gun_fires.erase(it);
      } else {
        ++it;
      }
    }

    /* Move enemies and remove out of frame enemies */
    for (vector<Plane>::iterator it = enemies.begin(); it != enemies.end(); ) {
      it->SetCenter(Point(it->GetCenter().GetX(), it->GetCenter().GetY() + it->GetYSpeed()));
      if (it->GetTopLeft().GetY() >= game_screen_bottom_right.GetY()) {
          it = enemies.erase(it);
      } else {
        ++it;
      }
    }

    /* Move background map */
    game_source_top_left.Translate(Point(0, -1));
    game_source_bottom_right.Translate(Point(0, -1));
    if (game_source_top_left.GetY() <= 0) {
      game_source_top_left.SetY(MAP_HEIGHT);
      game_source_bottom_right.SetY(MAP_HEIGHT + game_source_bottom_right.GetX() - game_source_top_left.GetX());
    }

    usleep(1000 / FPS * 1000);
  }

  if(died) {
    fb.Clear();
    main_screen.Render(fb);
    Font::RenderText("GAME OVER", font, fb, Point(main_screen_top_left.GetX() + 377, main_screen_top_left.GetY() + 368), COLOR_WHITE, COLOR_RED, COLOR_BLACK, 3, main_screen_top_left, main_screen_bottom_right);
    fb.Display();
    usleep(3000000);
    PlayCredits();
  }
}

/* Play the credits scene */
void PlayCredits() {
  /* Init */
  string names[10] = {
      "CREATED BY",
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

    Color names_color[10] = {
      COLOR_DEEP_SKY_BLUE,
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

 /* Starting ordinat for names */
 int start = main_screen_bottom_right.GetY();
 int scale = 3;

 /* Main loop */
 while (start >= -650) {
   fb.Clear();

   /* Draw names */
   for (int line = 0; line < 10; line++) {
     int len = names[line].length();
     int xoffset = main_screen_top_left.GetX() + ((MAIN_SCREEN_WIDTH - (len * (font.GetWidth() * scale + font.GetHorizontalSpace()) - font.GetHorizontalSpace())) / 2);
     int yoffset = start + line * (font.GetHeight() * scale + font.GetVerticalSpace());
     Font::RenderText(names[line], font, fb, Point(xoffset, yoffset), COLOR_WHITE, names_color[line], COLOR_BLACK, scale, main_screen_top_left, main_screen_bottom_right);
   }

   start -= 5;
   main_screen.Render(fb);
   fb.Display();
   usleep(1000 / FPS * 1000);
 }
}
