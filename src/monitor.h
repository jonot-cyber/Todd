#pragma once

#include "io.h"
#include "vga.h"

namespace Monitor {
    void init();
    void clear();

    void putChar(u8 c);
    void putString(u8* c);
    void putString(const i8* c);
    void resetColor();
    void writeHex(u8 c);
    void writeHex(u16 c);
    void writeHex(u32 c);
    void writeDec(u32 c);
    void writeBin(u32 c);
    void setPos(u8 x, u8 y, u8 c);
    void moveCursor();
    void scroll();
    u16 colorCharacter(u8 c);

    void setBackgroundColor(VGAColor c);
    void setForegroundColor(VGAColor c);
    VGAColor getBackgroundColor();
    VGAColor getForegroundColor();
};