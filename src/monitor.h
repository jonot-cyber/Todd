#pragma once

#include "io.h"
#include "vga.h"

class Monitor {
public:
    Monitor();

    void clear();

    void operator<<(u8 c);
    void operator<<(u16 c);
    void operator<<(u8* c);
    void operator<<(i8* c);
private:
    void moveCursor();
    void scroll();

    u8 cursorX;
    u8 cursorY;
};

void monitor_put(u8 c);
void monitor_clear();
void monitor_write(u8* c);