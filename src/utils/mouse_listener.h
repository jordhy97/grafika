#ifndef MOUSE_LISTENER_H
#define MOUSE_LISTENER_H

#include "../graphics/point.h"
#include <thread>

class MouseListener {
public:
  /* Constructor */
  MouseListener(const Point& frame_top_left, const Point& frame_bottom_right, const Point& position);

  /* Destructor */
  ~MouseListener();

  /* Predicate */
  bool IsLeftClicked() const;
  bool IsRightClicked() const;
  bool IsLeftClickPressed() const;
  bool IsRightClickPressed() const;
  bool IsLeftClickReleased() const;
  bool IsRightClickReleased() const;

  Point GetPosition() const;
  
private:
  void InputHandler();

  int device_;
  bool prev_left_click_;
  bool current_left_click_;
  bool prev_right_click_;
  bool current_right_click_;
  Point position_;
  Point frame_top_left_;
  Point frame_bottom_right_;
  bool active_;
  std::thread input_thread_;
};

#endif
