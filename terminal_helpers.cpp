//
// Created by Nathanael Thompson on 6/14/25.
//

#include "terminal_helpers.h"

#include <cstdlib>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>

// Store original terminal settings so we can restore them on exit
static struct termios orig_termios;

// Called on exit to reset terminal settings back to normal
void Terminal::disableRawMode() {
    // tcsetattr -> set the parameters associated with the terminal referred to by open file descriptor
    //              associated with the terminal
    // STDIN_FILENO -> constant that represents the file descriptor for standard input
    // TCSAFLUSH -> change occurs after all file descriptor output is transmitted,
    //              but discards any input received and not read
    // &orig_termios -> reference to the original terminal settings
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

// Enables "raw mode" on the terminal:
// - Turns off canonical mode (i.e., input is available immediately, not line-buffered)
// - Turns off echo (characters typed aren't printed to screen)
void Terminal::enableRawMode() {
    // get current terminal attributes and save them
    tcgetattr(STDIN_FILENO, &orig_termios);

    // ensure terminal gets restored when program exits
    atexit(disableRawMode);

    // make a copy to modify
    struct termios raw = orig_termios;

    // ICANON: disables line buffering
    // ECHO: disabes character echoing
    raw.c_lflag &= ~(ICANON | ECHO);

    // apply modified settings immediately
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int Terminal::keyboardHit() {
    // no wait
    struct timeval tv = {0,0};

    // (read) file descriptor set
    fd_set readFileDescriptors;

    // clear the set
    FD_ZERO(&readFileDescriptors);

    // add standard in to the set
    FD_SET(STDIN_FILENO, &readFileDescriptors);

    // select returns > 0 if input is available on stdin
    return select(STDIN_FILENO + 1, &readFileDescriptors, NULL, NULL, &tv) > 0;
}

char Terminal::getCharacter() {
    char c;
    read(STDIN_FILENO, &c, 1);
    return c;
}

