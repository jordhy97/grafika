#include "input.h"
#include <fcntl.h>
#include <unistd.h>

/* Constructor */
Input::Input() {
  /* get the terminal settings for stdin */
  tcgetattr(STDIN_FILENO, &tio_);

  /* disable canonical mode (buffered i/o) and local echo */
  tio_.c_lflag &= (~ICANON & ~ECHO);

  /* set the new settings immediately */
  tcsetattr(STDIN_FILENO, TCSANOW, &tio_);

  /* make the reads non-blocking */
  fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
}

/* Destructor */
Input::~Input() {
  /* enable canonical mode (buffered i/o) and local echo */
  tio_.c_lflag |= (ICANON | ECHO);

  /* set the new settings immediately */
  tcsetattr(STDIN_FILENO, TCSANOW, &tio_);
}

/* Get the pressed key */
char Input::GetKeyPressed() {
  char c = 'D';
  read(STDIN_FILENO, &c, 1);
  return c;
}

/* Return true if the key is pressed else false */
bool Input::IsKeyPressed(char key) {
  return (GetKeyPressed() == key);
}

/* Flush input */
void Input::Flush() {
  tcflush(0, TCIFLUSH);
}
