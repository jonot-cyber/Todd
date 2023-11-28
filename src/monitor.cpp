#include "monitor.h"

static VGAColor backgroundColor = VGAColor::Black;
static VGAColor foregroundColor = VGAColor::White;

static u8 cursorX;
static u8 cursorY;

u16* videoMemory = (u16*)0xB8000;

/* Initializes the monitor */
void Monitor::init() {
    cursorX = 0;
    cursorY = 0;
    resetColor();
    clear();
}

/* Move the cursor to the correct position */
void Monitor::moveCursor() {
    u16 cursorLocation = cursorY * 80 + cursorX;

    u16 cmdPort = 0x3D4;
    u16 dataPort = 0x3D5;

    // Send lower bit
    IO::out(cmdPort, 14);
    IO::out(dataPort, cursorLocation >> 8);

    // Send upper bit
    IO::out(cmdPort, 15);
    IO::out(dataPort, cursorLocation);
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
void Monitor::putChar(u8 c) {
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
void Monitor::putString(u8 *c) {
    for (int i = 0; c[i]; i++) {
        Monitor::putChar(c[i]);
    }
}

/* Also outputs a string to the screen. Used because chars are signed in C++ */
void Monitor::putString( const i8 *c) {
    for (int i = 0; c[i]; i++) {
        Monitor::putChar(static_cast<u8>(c[i]));
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
    return c | (attr << 8);
}

void Monitor::writeHex(u8 c) {
    const char hexDigits[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    u8 hi = c >> 4;
    u8 low = c & 0xF;
    putChar(hexDigits[hi]);
    putChar(hexDigits[low]);
}

void Monitor::writeHex(u16 c) {
    u8 hi = c >> 8;
    u8 lo = c;
    writeHex(hi);
    writeHex(lo);
}

void Monitor::writeHex(u32 c) {
    u16 hi = c >> 16;
    u16 lo = c;
    writeHex(hi);
    writeHex(lo);
}

void Monitor::writeDec(u32 c) {
    if (c == 0) {
        putChar('0');
        return;
    }
    u32 x = 1;
    while (c > x) {
        x *= 10;
    }
    while (x > 1) {
        x /= 10;
        u32 part = (c / x) % 10;
        putChar(part + '0');
    }
}

void Monitor::writeBin(u32 c) {
    for (int i = 31; i >= 0; i--) {
        u8 value = (c >> i) & 1;
        putChar(value ? '1' : '0');
    }
}

void Monitor::setPos(u8 x, u8 y, u8 c) {
    u8 oldX = cursorX;
    u8 oldY = cursorY;
    cursorX = x;
    cursorY = y;
    putChar(c);
    cursorX = oldX;
    cursorY = oldY;
}

void Monitor::setBackgroundColor(VGAColor c) {
    backgroundColor = c;
}

void Monitor::setForegroundColor(VGAColor c) {
    foregroundColor = c;
}

VGAColor Monitor::getBackgroundColor() {
    return backgroundColor;
}

VGAColor Monitor::getForegroundColor() {
    return foregroundColor;
}