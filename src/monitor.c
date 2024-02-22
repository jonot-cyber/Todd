#include "monitor.h"

#include "io_port.h"
#include "mutex.h"
#include "queue.h"

enum VGAColor background_color = BLACK;
enum VGAColor foreground_color = WHITE;

static u8 cursorX;
static u8 cursorY;

u16* videoMemory = (u16*)0xB8000;

/* A queue to hold characters to print. This allows multiple threads
 * to output text without starvation with the mutex */
static struct Queue write_queue;
static mutex_t write_lock = 0;

void clear();
u16 color_character(u8);
void reset_color();
void move_cursor();
void set_pos(u8, u8, u8);

void monitor_init() {
	queue_init(&write_queue, 64);
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

/* Output a character without multi-thread considerations */
void monitor_write_char(i8 c) {
	switch (c) {
	case '\b':
		if (cursorX != 0) {
			cursorX--;
			set_pos(cursorX, cursorY, ' ');
		}
		break;
	case '\t':
		cursorX = (cursorX + 8) & ~7;
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
		u16* location = videoMemory + (cursorX + cursorY * 80);
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
	u8 oldX = cursorX;
	u8 oldY = cursorY;
	cursorX = x;
	cursorY = y;
	monitor_write_char(c);
	cursorX = oldX;
	cursorY = oldY;
}
