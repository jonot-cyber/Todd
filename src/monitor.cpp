#include "monitor.h"

#include "io.h"
#include "common.h"

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
		for (u32 i = 0; i < 24 * 80; i++) {
			videoMemory[i] = videoMemory[i+80];
		}

		for (u32 i = 24 * 80; i < 25 * 80; i++) {
			videoMemory[i] = blank;
		}

		cursorY = 24;
	}
}

/* Output a character to the screen */
void Monitor::writeChar(i8 c) {
	switch (c) {
        case '\b':
		if (cursorX != 0) {
			cursorX--;
			Monitor::setPos(cursorX, cursorY, ' ');
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

	for (u32 i = 0; i < 80*25; i++) {
		videoMemory[i] = blank;
	}

	// Move the hardware cursor back to the start.
	cursorX = 0;
	cursorY = 0;
	moveCursor();
}

/* Also outputs a string to the screen. Used because chars are signed in C++ */
void Monitor::writeString( const i8 *c) {
	for (u32 i = 0; c[i]; i++) {
		writeChar((u8)c[i]);
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
	writeChar(hexDigits[hi]);
	writeChar(hexDigits[low]);
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
	if (c < 10) {
		writeChar('0'+c);
		return;
	}
	writeDec(c/10);
	writeDec(c % 10);
}

void Monitor::writeBin(u32 c) {
	for (u32 i = 0; i < 32; i++) {
		u8 value = (c >> (31-i)) & 1;
		writeChar('0' + value);
	}
}

void Monitor::setPos(u8 x, u8 y, u8 c) {
	u8 oldX = cursorX;
	u8 oldY = cursorY;
	cursorX = x;
	cursorY = y;
	writeChar(c);
	cursorX = oldX;
	cursorY = oldY;
}

u8 hex2num(i8 c) {
	if (c >= '0' && c <= '9') {
		return c - '0';
	}
	if (c >= 'a' && c <= 'f') {
		return c - 'a' + 10;
	}
	return c - 'A' + 10;
}

void Monitor::printf(const i8* str) {
	while (*str != '\0') {
		if (*str == '%') {
			if (*(str+1) == 'C') {
				switch (*(str+2)) {
				case 'r':
					Monitor::resetColor();
					str += 3;
					continue;
				case 'f': {
					i8 c = *(str+3);
					u8 v;
					str += 4;
					v = hex2num(c);
					Monitor::setForegroundColor((VGAColor)v);
					continue;
				}
				case 'b': {
					i8 c = *(str+3);
					u8 v;
					str += 4;
					v = hex2num(c);
					Monitor::setBackgroundColor((VGAColor)v);
					continue;
				}
				}
				if (*(str+2) == 'r') {
					Monitor::resetColor();
					str += 3;
					continue;
				}
			}
		}		
		writeChar(*str);
		str++;
	}
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
