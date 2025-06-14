//
// Created by Nathanael Thompson on 6/14/25.
//

#ifndef TERMINAL_HELPERS_H
#define TERMINAL_HELPERS_H

namespace Terminal {
    void enableRawMode();
    void disableRawMode();
    int keyboardHit();
    char getCharacter();
}

#endif //TERMINAL_HELPERS_H
