#include "monitor.h"

#include "io_port.h"
#include "mutex.h"
#include "queue.h"

enum VGAColor background_color = BLACK;
enum VGAColor foreground_color = WHITE;

static u8 cursor_x = 0;
static u8 cursor_y = 0;

volatile u16 *video_memory = (u16*)0xB8000;

void clear();
u16 color_character(u8);
void reset_color();
void move_cursor();
void set_pos(u8, u8, u8);

void monitor_init() {
	clear();
}

void clear() {
	reset_color();
	u16 blank = color_character(' ');

	for (u32 i = 0; i < 80*25; i++) {
		video_memory[i] = blank;
	}

	// Move the hardware cursor back to the start.
	cursor_x = 0;
	cursor_y = 0;
	move_cursor();
}

void move_cursor() {
	u16 cursor_loc = cursor_y * 80 + cursor_x;

	const u16 CMD_PORT = 0x3D4;
	const u16 DATA_PORT = 0x3D5;

	// Send lower bit
	io_out(CMD_PORT, 14);
	io_out(DATA_PORT, cursor_loc >> 8);

	// Send upper bit
	io_out(CMD_PORT, 15);
	io_out(DATA_PORT, cursor_loc);
}

/* Scroll the screen if there is no space left over */
void scroll() {
	u16 blank = color_character(' ');

	if (cursor_y >= 25) {
		for (u32 i = 0; i < 24 * 80; i++)
			video_memory[i] = video_memory[i+80];

		for (u32 i = 24 * 80; i < 25 * 80; i++)
			video_memory[i] = blank;

		cursor_y = 24;
	}
}

/* Output a character without multi-thread considerations */
void monitor_write_char(i8 c) {
	switch (c) {
	case '\b':
		if (cursor_x != 0) {
			cursor_x--;
			set_pos(cursor_x, cursor_y, ' ');
		}
		break;
	case '\t':
		cursor_x = (cursor_x + 8) & ~7;
		break;
	case '\r':
		cursor_x = 0;
		break;
	case '\n':
		cursor_x = 0;
		cursor_y++;
		break;
	default:
	{
		volatile u16 *location = video_memory + (cursor_x + cursor_y * 80);
		*location = color_character(c);
		cursor_x++;
		if (cursor_x == 80) {
			cursor_x = 0;
			cursor_y++;
		}
		break;
	}
	}
	scroll();
	move_cursor();
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

void set_pos(u8 x, u8 y, u8 c) {
	u8 oldX = cursor_x;
	u8 oldY = cursor_y;
	cursor_x = x;
	cursor_y = y;
	monitor_write_char(c);
	cursor_x = oldX;
	cursor_y = oldY;
}
