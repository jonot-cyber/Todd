#include "monitor.h"

u16* videoMemory = (u16*)0xB8000;

/* Initializes the monitor */
Monitor::Monitor() {
    cursorX = 0;
    cursorY = 0;
    resetColor();
    clear();
}

/* Move the cursor to the correct position */
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

/* Scroll the screen if there is no space left over */
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

/* Output a character to the screen */
void Monitor::operator<<(u8 c) {
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
            u16* location = videoMemory + (cursorY * 80 + cursorX);
            *location = colorCharacter(c);
            cursorX++;
            break;
    }
    scroll();
    moveCursor();
}

/* Clear the screen */
void Monitor::clear() {
   u16 blank = colorCharacter(' ');

   for (int i = 0; i < 80*25; i++) {
       videoMemory[i] = blank;
   }

   // Move the hardware cursor back to the start.
   cursorX = 0;
   cursorY = 0;
   moveCursor();
}

/* Output a string to the screen */
void Monitor::operator<<(u8* c) {
    for (int i = 0; c[i]; i++) {
        *this << c[i];
    }
}

/* Also outputs a string to the screen. Used because chars are signed in C++ */
void Monitor::operator<<(const i8* c) {
    for (int i = 0; c[i]; i++) {
        *this << *(u8*)&c[i];
    }
}

/* Reset the colors */
void Monitor::resetColor() {
    foregroundColor = VGAColor::White;
    backgroundColor = VGAColor::Black;
}

/* Color a character */
u16 Monitor::colorCharacter(u8 c) {
    u8 attr = ((u8)backgroundColor << 4) | ((u8)foregroundColor & 0x0F);
    u16 wc = c | (attr << 8);
    return wc;
}

void Monitor::writeHex(u8 c) {
    const char hexDigits[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    u8 hi = c >> 4;
    u8 low = c & 0xF;
    *this << hexDigits[hi];
    *this << hexDigits[low];
}
