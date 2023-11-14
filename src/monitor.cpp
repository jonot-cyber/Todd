#include "monitor.h"

u16* videoMemory = (u16*)0xB8000;

Monitor::Monitor() {
    cursorX = 0;
    cursorY = 0;
}

void Monitor::moveCursor() {
    u16 cursorLocation = cursorY * 80 + cursorX;
    
    IO commandPort(0x3D4);
    IO dataPort(0x3D5);

    // Send lower bit
    commandPort << (u8)14;
    dataPort << (u8)(cursorLocation >> 8);

    // Send upper bit
    commandPort << (u8)15;
    dataPort << (u8)(cursorLocation);
}

void Monitor::scroll() {
    u16 blank = colorCharacter(' ');

    if (cursorY >= 25) {
        for (int i = 0; i < 24 * 80; i++) {
            videoMemory[i] = videoMemory[i+80];
        }

        for (int i = 24 * 80; i < 25 * 80; i++) {
            videoMemory[i] = blank;
        }

        cursorY = 24;
    }
}

void Monitor::operator<<(u16 c) {
    u16 wc = c;

    u16* loc;
    switch (c) {
        case '\b':
            if (cursorX != 0) {
                cursorX--;
            }
            break;
        case '\t':
            cursorX = (cursorX + 8) & ~(8-1);
            break;
        case '\r':
            cursorX = 0;
            break;
        case '\n':
            cursorX = 0;
            cursorY++;
            break;
        default:
            loc = videoMemory + (cursorY * 80 + cursorX);
            *loc = wc;
            cursorX++;
            break;
    }
    scroll();
    moveCursor();
}

void Monitor::operator<<(u8 c) {
    *this << colorCharacter(c);
}

void Monitor::clear() {
   u16 blank = colorCharacter(' ');

   int i;
   for (i = 0; i < 80*25; i++)
   {
       videoMemory[i] = blank;
   }

   // Move the hardware cursor back to the start.
   cursorX = 0;
   cursorY = 0;
   moveCursor();
}

void Monitor::operator<<(u8* c) {
    for (int i = 0; c[i]; i++) {
        *this << c[i];
    }
}

void Monitor::operator<<(i8* c) {
    for (int i = 0; c[i]; i++) {
        *this << reinterpret_cast<i8>(c[i]);
    }
}