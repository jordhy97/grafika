#include "mouse_listener.h"
#include <unistd.h>
#include <fcntl.h>

/* Constructor */
MouseListener::MouseListener(const Point& frame_top_left, const Point& frame_bottom_right, const Point& position) {
  const char *device_path = "/dev/input/mice";
  device_ = open(device_path, O_RDWR);
  if (device_ == -1) {
    perror("Error: cannot open mouse");
    exit(7);
  }

  prev_left_click_ = false;
  current_left_click_ = false;
  prev_right_click_ = false;
  current_right_click_ = false;
  frame_top_left_ = frame_top_left;
  frame_bottom_right_ = frame_bottom_right;
  position_ = position;

  /* make the reads non-blocking */
  fcntl(device_, F_SETFL, O_NONBLOCK);

  active_ = true;
  input_thread_ = std::thread(&MouseListener::InputHandler, this);
}

/* Destructor */
MouseListener::~MouseListener() {
  active_ = false;
  input_thread_.join();
}

/* Predicate */
bool MouseListener::IsLeftClicked() const{
  return current_left_click_;
}

bool MouseListener::IsRightClicked() const{
  return current_right_click_;
}

bool MouseListener::IsLeftClickPressed() const{
  return !prev_left_click_ && current_left_click_;
}

bool MouseListener::IsRightClickPressed() const{
  return !prev_right_click_ && current_right_click_;
}

bool MouseListener::IsLeftClickReleased() const{
  return prev_left_click_ && !current_left_click_;
}

bool MouseListener::IsRightClickReleased() const{
  return prev_right_click_ && !current_right_click_;
}

Point MouseListener::GetPosition() const {
  return position_;
}

void MouseListener::InputHandler() {
  int bytes;
  unsigned char data[3];

  while (active_) {
    bytes = read(device_, data, sizeof(data));
    if(bytes > 0) {
      prev_left_click_ = current_left_click_;
      prev_right_click_ = current_right_click_;
      current_left_click_ = data[0] & 0x1;
      current_right_click_ = data[0] & 0x2;

      position_.SetX(position_.GetX() + (int) ((char) data[1]));
      position_.SetY(position_.GetY() - (int) ((char) data[2]));

      if (position_.GetX() < frame_top_left_.GetX()) {
        position_.SetX(frame_top_left_.GetX());
      }
      if (position_.GetX() > frame_bottom_right_.GetX()) {
        position_.SetX(frame_bottom_right_.GetX());
      }
      if (position_.GetY() < frame_top_left_.GetY()) {
        position_.SetY(frame_top_left_.GetY());
      }
      if (position_.GetY() > frame_bottom_right_.GetY()) {
        position_.SetY(frame_bottom_right_.GetY());
      }
    }
  }
}
