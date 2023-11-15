#pragma once

#include "io.h"
#include "vga.h"

class Monitor {
public:
    Monitor();

    void clear();

    void operator<<(u8 c);
    void operator<<(u8* c);
    void operator<<(const i8* c);
    void resetColor();

    VGAColor backgroundColor;
    VGAColor foregroundColor;
private:
    void moveCursor();
    void scroll();
    u16 colorCharacter(u8 c);

    u8 cursorX;
    u8 cursorY;
};