//
// Created by Nathanael Thompson on 6/14/25.
//

#include "tetris.h"

#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "terminal_helpers.h"

void draw();
bool isCurrentPieceAt(int x, int y);
void tick();
void lockPiece();
bool canMove(int dx, int dy);
void moveLeft();
void moveRight();
void rotate();
bool canRotate(int newRotation);
void drop();
void clearLines();
bool checkGameOver();
std::string getColorCode(int pieceIndex) {
    switch (pieceIndex) {
        case 0: return "\033[36m"; // I - cyan
        case 1: return "\033[34m"; // J - blue
        case 2: return "\033[33m"; // L - yellow/orange
        case 3: return "\033[32m"; // O - green
        case 4: return "\033[31m"; // S - red
        case 5: return "\033[35m"; // T - magenta
        case 6: return "\033[91m"; // Z - bright red
        default: return "\033[0m"; // reset
    }
}

const int WIDTH = 10;
const int HEIGHT = 20;

// pieces
const std::vector<std::vector<std::vector<std::string>>> tetrisPieces = {
    // I
    {
        {
            "....",
            "####",
            "....",
            "...."
        },
        {
            "..#.",
            "..#.",
            "..#.",
            "..#."
        },
        {
            "....",
            "....",
            "####",
            "...."
        },
        {
            ".#..",
            ".#..",
            ".#..",
            ".#.."
        }
    },

    // J
    {
        {
            "#...",
            "###.",
            "....",
            "...."
        },
        {
            ".##.",
            ".#..",
            ".#..",
            "...."
        },
        {
            "....",
            "###.",
            "..#.",
            "...."
        },
        {
            ".#..",
            ".#..",
            "##..",
            "...."
        }
    },

    // L
    {
        {
            "..#.",
            "###.",
            "....",
            "...."
        },
        {
            ".#..",
            ".#..",
            ".##.",
            "...."
        },
        {
            "....",
            "###.",
            "#...",
            "...."
        },
        {
            "##..",
            ".#..",
            ".#..",
            "...."
        }
    },

    // O (square)
    {
        {
            ".##.",
            ".##.",
            "....",
            "...."
        },
        {
            ".##.",
            ".##.",
            "....",
            "...."
        },
        {
            ".##.",
            ".##.",
            "....",
            "...."
        },
        {
            ".##.",
            ".##.",
            "....",
            "...."
        }
    },

    // S
    {
        {
            ".##.",
            "##..",
            "....",
            "...."
        },
        {
            ".#..",
            ".##.",
            "..#.",
            "...."
        },
        {
            "....",
            ".##.",
            "##..",
            "...."
        },
        {
            "#...",
            "##..",
            ".#..",
            "...."
        }
    },

    // T
    {
        {
            ".#..",
            "###.",
            "....",
            "...."
        },
        {
            ".#..",
            ".##.",
            ".#..",
            "...."
        },
        {
            "....",
            "###.",
            ".#..",
            "...."
        },
        {
            ".#..",
            "##..",
            ".#..",
            "...."
        }
    },

    // Z
    {
        {
            "##..",
            ".##.",
            "....",
            "...."
        },
        {
            "..#.",
            ".##.",
            ".#..",
            "...."
        },
        {
            "....",
            "##..",
            ".##.",
            "...."
        },
        {
            ".#..",
            "##..",
            "#...",
            "...."
        }
    }
};


// board
std::vector<std::vector<int>> board(HEIGHT, std::vector<int>(WIDTH, 0));

// active piece state
int currentX = 3, currentY = 0;
int currentPiece = 0;
int rotation = 0;

// core functions
void draw() {
    // ANSI escape code: move cursor to top-left
    std::cout << "\033[H";

    // top border
    std::cout << "┌";

    for (int i = 0; i < WIDTH; i++) {
        std::cout << "-";
    }

    std::cout << "┐\n";

    for (int y = 0; y < HEIGHT; y++) {
        std::cout << "│";
        for (int x = 0; x < WIDTH; x++) {
            // draw locked blocks
            if (board[y][x] != 0) {
                std::cout << "#";
            }

            // draw active, unlocked piece
            else if (isCurrentPieceAt(x, y)) {
                std::cout << getColorCode(currentPiece) << "■" << "\033[0m";
            }
            else {
                std::cout << " ";
            }
        }

        std::cout << "│\n";
    }

    // bottom border
    std::cout << "└";
    for (int i = 0; i < WIDTH; ++i) std::cout << "─";
    std::cout << "┘\n";

    // controls
    std::cout << "\nControls: A/Left D/Right S/Drop W/Rotate Q/Quit \n";
}

bool isCurrentPieceAt(int x, int y) {
    for (int py = 0; py < 4; ++py) {
        for (int px = 0; px < 4; ++px) {
            if (tetrisPieces[currentPiece][rotation][py][px] == '#' &&
                (currentX + px == x) &&
                (currentY + py == y)) {
                return true;
            }
        }
    }

    return false;
}

void tick() {
    if (canMove(0, 1)) {
        currentY += 1; // gravity drop
    }
    else {
        lockPiece();

        // reset to top with new piece
        currentPiece = rand() % tetrisPieces.size();
        currentX = 3;
        currentY = 0;
        rotation = 0;
    }
}

// checks whether the active piece can move by directions dx and dy
bool canMove(int dx, int dy) {
    for (int py = 0; py < 4; ++py) {
        for (int px = 0; px < 4; ++px) {
            if (tetrisPieces[currentPiece][rotation][py][px] == '#') {
                int newX = currentX + px + dx;
                int newY = currentY + py + dy;

                // out of bounds?
                if (newX < 0 || newX >= WIDTH || newY < 0 || newY >= HEIGHT) {
                    return false;
                }

                // collides with locked block?
                if (board[newY][newX] != 0) {
                    return false;
                }
            }
        }
    }

    return true;
}

// merge current piece into the board
void lockPiece() {
    for (int py = 0; py < 4; ++py) {
        for (int px = 0; px < 4; ++px) {
            if (tetrisPieces[currentPiece][rotation][py][px] == '#') {
                // board coordinates
                int bx = currentX + px;
                int by = currentY + py;

                if (by >= 0 && by < HEIGHT && bx >= 0 && bx < WIDTH) {
                    board[by][bx] = 1;
                }
            }
        }
    }

    clearLines();
}

void moveLeft() {
    if (canMove(-1, 0)) {
        currentX -= 1;
    }
}

void moveRight() {
    if (canMove(1, 0)) {
        currentX += 1;
    }
}

void drop() {
    while (canMove(0, 1)) {
        currentY += 1;
    }

    tick(); // locks the piece and spawns a new one
    // TODO: implement soft drop
}

void rotate() {
    int newRotation = (rotation + 1) % 4;
    if (canRotate(newRotation)) {
        rotation = newRotation;
    }
}

bool canRotate(int newRotation) {
    for (int py = 0; py < 4; ++py) {
        for (int px = 0; px < 4; ++px) {
            if (tetrisPieces[currentPiece][newRotation][py][px] == '#') {
                int newX = currentX + px;
                int newY = currentY + py;

                // Out of bounds
                if (newX < 0 || newX >= WIDTH || newY < 0 || newY >= HEIGHT)
                    return false;

                // Blocked
                if (board[newY][newX] != 0)
                    return false;
            }
        }
    }
    return true;
}

void clearLines() {
    for (int y = 0; y < HEIGHT; y++) {
        bool full = true;
        for (int x = 0; x < WIDTH; x++) {
            if (board[y][x] == 0) {
                full = false;
                break;
            }
        }

        if (full) {
            board.erase(board.begin() + y);
            board.insert(board.begin(), std::vector<int>(WIDTH, 0));

            --y; // check the same index after shifting
        }
    }
}

bool checkGameOver() {
    return !canMove(0, 0);
}


int main() {
    Terminal::enableRawMode();
    bool running = true;
    std::cout << "\033[2J\033[H"; // clear screen

    const int tickDelayMs = 500;
    const int frameDelayMs = 1000/60;
    auto lastTick = std::chrono::steady_clock::now();

    while (running) {
        auto now = std::chrono::steady_clock::now();
        auto msSinceTick = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTick);

        // Handle input every frame
        if (Terminal::keyboardHit()) {
            char ch = Terminal::getCharacter();
            switch (ch) {
                case 'q': running = false; break;
                case 'w': rotate(); break;
                case 'a': moveLeft(); break;
                case 'd': moveRight(); break;
                case 's': drop(); break;
            }
        }

        // Run game logic (tick) only when enough time has passed
        if (msSinceTick.count() >= tickDelayMs) {
            tick();
            lastTick = now;
        }

        // Draw every frame (always show the latest state)
        draw();

        // Check for game over
        if (checkGameOver()) {
            std::cout << "\033[2J\033[H"; // clear screen
            std::cout << "Game Over!\n";
            std::this_thread::sleep_for(std::chrono::seconds(2));
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(frameDelayMs));
    }

    std::cout << "\033[2J\033[H"; // clear screen
    Terminal::disableRawMode();
    return 0;
}