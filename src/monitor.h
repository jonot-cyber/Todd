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

	void printf(const i8*);

	template <typename T, typename... Args>
	void printf(const i8* str, T value, Args... args) {
		while (*str) {
			if (*str == '%') {
				i8 next = *(str+1);
				switch (next) {
				case '%':
					Monitor::putChar('%');
					break;
				case 'b':
				{
					// Print a binary
					u32 v = (u32)value;
					Monitor::writeBin(v);
					break;
				}
				case 'd':
				{
					// Print a decimal
					u32 v = (u32)value;
					Monitor::writeDec(v);
					break;
				}
				case 'x':
				{
					// Print a hex
					u32 v = (u32)value;
					Monitor::writeHex(v);
					break;
				}
				case 's':
				{
					// Print a string
					const i8* v = (const i8*)value;
					Monitor::putString(v);
					break;
				}
				case 'c':
				{
					u32 v = (u32)value;
					Monitor::putChar(v);
					break;
				}
				}
				str += 2;
				// Recurse the function
				printf(str, args...);
				return;
			} else {
				Monitor::putChar(*str);
			}
			str++;
		}
	}

    void setBackgroundColor(VGAColor c);
    void setForegroundColor(VGAColor c);
    VGAColor getBackgroundColor();
    VGAColor getForegroundColor();
};
