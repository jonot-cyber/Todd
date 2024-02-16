#include "monitor.h"

#include "io.h"

enum VGAColor background_color = BLACK;
enum VGAColor foreground_color = WHITE;

static u8 cursorX;
static u8 cursorY;

u16* videoMemory = (u16*)0xB8000;

void clear();
u16 color_character(u8);
void reset_color();
void move_cursor();
void set_pos(u8, u8, u8);

void monitor_init() {
	cursorX = 0;
	cursorY = 0;
	reset_color();
	clear();
}

void clear() {
	reset_color();
	u16 blank = color_character(' ');

	for (u32 i = 0; i < 80*25; i++) {
		videoMemory[i] = blank;
	}

	// Move the hardware cursor back to the start.
	cursorX = 0;
	cursorY = 0;
	move_cursor();
}

void move_cursor() {
	u16 cursorLocation = cursorY * 80 + cursorX;

	u16 cmdPort = 0x3D4;
	u16 dataPort = 0x3D5;

	// Send lower bit
	io_out(cmdPort, 14);
	io_out(dataPort, cursorLocation >> 8);

	// Send upper bit
	io_out(cmdPort, 15);
	io_out(dataPort, cursorLocation);
}

/* Scroll the screen if there is no space left over */
void scroll() {
	u16 blank = color_character(' ');

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
void write_char(i8 c) {
	switch (c) {
        case '\b':
		if (cursorX != 0) {
			cursorX--;
			set_pos(cursorX, cursorY, ' ');
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
	{
		u16* location = videoMemory + (cursorY * 80 + cursorX);
		*location = color_character(c);
		cursorX++;
		if (cursorX == 80) {
			cursorX = 0;
			cursorY++;
		}
		break;
	}
	}
	scroll();
	move_cursor();
}

void write_string( const i8 *c) {
	for (u32 i = 0; c[i]; i++) {
		write_char((u8)c[i]);
	}
}

/* Reset the colors */
void reset_color() {
	foreground_color = WHITE;
	background_color = BLACK;
}

u16 color_character(u8 c) {
	u8 attr = ((u8)background_color << 4) | ((u8)foreground_color & 0x0F);
	return c | (attr << 8);
}

void write_hex(u8 c) {
	const char hexDigits[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
	u8 hi = c >> 4;
	u8 low = c & 0xF;
	write_char(hexDigits[hi]);
	write_char(hexDigits[low]);
}

void write_hex16(u16 c) {
	u8 hi = c >> 8;
	u8 lo = c;
	write_hex(hi);
	write_hex(lo);
}

void write_hex32(u32 c) {
	u16 hi = c >> 16;
	u16 lo = c;
	write_hex16(hi);
	write_hex16(lo);
}

void write_dec(u32 c) {
	if (c < 10) {
		write_char('0'+c);
		return;
	}
	write_dec(c / 10);
	write_dec(c % 10);
}

void write_bin(u32 c) {
	for (u32 i = 0; i < 32; i++) {
		u8 value = (c >> (31-i)) & 1;
		write_char('0' + value);
	}
}

void set_pos(u8 x, u8 y, u8 c) {
	u8 oldX = cursorX;
	u8 oldY = cursorY;
	cursorX = x;
	cursorY = y;
	write_char(c);
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

void printf(const i8* str, ...) {
	u32 stack_ptr = (u32)&str;
	u32 offset = 4;
	while (*str) {
		if (*str == '%') {
			i8 next = *(str + 1);
			switch (next) {
			case '%':
				write_char('%');
				break;
			case 'b':
			{
				u32 v = *(u32*)(stack_ptr+offset);
				write_bin(v);
				offset += 4;
				break;
			}
			case 'd':
			{
				u32 v = *(u32*)(stack_ptr + offset);
				write_dec(v);
				offset += 4;
				break;
			}
			case 'x':
			{
				u32 v = *(u32*)(stack_ptr + offset);
				write_hex32(v);
				offset += 4;
				break;
			}
			case 's':
			{
				const i8* v = *(const i8**)(stack_ptr + offset);
				write_string(v);
				offset += 4;
				break;
			}
			case 'c':
			{
				u32 v = *(u32*)(stack_ptr + offset);
				write_char(v);
				offset += 4;
				break;
			}
			case 'C':
			{
				i8 next = *(str+2);
				if (next == 'f') {
					i8 c = *(str+3);
					u8 v;
					if (c >= '0' && c <= '9') {
						v = c - '0';
					} else if (c >= 'A' && c <= 'F') {
						v = c - 'A' + 10;
					} else {
						v = c - 'a' + 10;
					}
					foreground_color = (enum VGAColor)v;
					str += 2;
				} else if (next == 'b') {
					i8 c = *(str+3);
					u8 v;
					if (c >= '0' && c <= '9') {
						v = c - '0';
					} else if (c >= 'A' && c <= 'F') {
						v = c - 'A' + 10;
					} else {
						v = c - 'a' + 10;
					}
					background_color = (enum VGAColor)v;
					str += 2;
				} else if (next == 'r') {
					reset_color();
					str += 1;
				}
			}
			}
			str += 2;
		} else {
			write_char(*str);
			str++;
		}
	}
}
