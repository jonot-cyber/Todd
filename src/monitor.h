#pragma once

#include "string.h"
#include "vga.h"

namespace Monitor {
	/**
	   Initialize the monitor.
	 */
	void init();

	/**
	   Clear the monitor.
	 */
	void clear();

	/**
	   Write a character

	   @param c is the character
	*/
	void writeChar(i8 c);
	
	/**
	   Write a string

	   @param c is the string
	 */
	void writeString(const i8* c);
	
	/**
	   Reset the color to white/black
	 */
	void resetColor();

	/**
	   Write a hexadecimal byte

	   @param c is a byte
	 */
	void writeHex(u8 c);

	/**
	   Write a hexadecimal word

	   @param c is a word
	 */
	void writeHex(u16 c);

	/**
	   Write a hexadecimal dword

	   @param c is a integer
	 */
	void writeHex(u32 c);

	/**
	   Write a decimal dword

	   @param c is an integer
	 */
	void writeDec(u32 c);

	/**
	   Write a binary dword

	   @param c is a dword
	 */
	void writeBin(u32 c);

	/**
	   Set a character at a position

	   @param x is the x position
	   @param y is the y position
	 */
	void setPos(u8 x, u8 y, u8 c);

	/**
	   Move the cursor where it should be
	 */
	void moveCursor();

	/**
	   Scroll the screen if it needs to be scrolled
	 */
	void scroll();

	/**
	   Color a character for VGA

	   @param c is a character
	 */
	u16 colorCharacter(u8 c);

	/**
	   Formatted print: base case.

	   Formats: %% to print a %
	   %s to print a string
	   %s to print a dynamic "String"
	   %c to print a character
	   %d to print a decimal number
	   %b to print a binary number
	   %x to print a hexadecimal number

	   @param str is a string to output
	 */
	void printf(const i8* str);

	template <typename T, typename... Args>
	void printf(const i8* str, T value, Args... args) {
		while (*str) {
			if (*str == '%') {
				i8 next = *(str+1);
				switch (next) {
				case '%':
					Monitor::writeChar('%');
					break;
				case 'b':
				{
					// Print a binary
					u32 v = *(u32*)&value;
					Monitor::writeBin(v);
					break;
				}
				case 'd':
				{
					// Print a decimal
					u32 v = *(u32*)&value;
					Monitor::writeDec(v);
					break;
				}
				case 'x':
				{
					// Print a hex
					u32 v = *(u32*)&value;
					Monitor::writeHex(v);
					break;
				}
				case 's':
				{
					// Print a string
					const i8* v = *(const i8**)&value;
					Monitor::writeString(v);
					break;
				}
				case 'S':
				{
					Monitor::writeString((*(Types::String*)&value).data);
					break;
				}
				case 'c':
				{
					u32 v = *(u32*)&value;
					Monitor::writeChar(v);
					break;
				}
				}
				str += 2;
				// Recurse the function
				printf(str, args...);
				return;
			} else {
				Monitor::writeChar(*str);
			}
			str++;
		}
	}

	void setBackgroundColor(VGAColor c);
	void setForegroundColor(VGAColor c);
	VGAColor getBackgroundColor();
	VGAColor getForegroundColor();
};
