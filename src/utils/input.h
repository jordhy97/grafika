#ifndef INPUT_H
#define INPUT_H

#include <termios.h>

class Input {
public:
  /* Constructor */
  Input();

  /* Destructor */
  ~Input();

  /* Get the pressed key */
  char GetKeyPressed();

  /* Return true if the key is pressed else false */
  bool IsKeyPressed(char key);

  /* Flush input */
  void Flush();

private:
    struct termios tio_;
};

#endif
