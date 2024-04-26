#ifndef USR_INCLUDE_STDIO_H
#define USR_INCLUDE_STDIO_H

enum VGAColor {
	VGA_BLACK,
	VGA_BLUE,
	VGA_GREEN,
	VGA_CYAN,
	VGA_RED,
	VGA_MAGENTA,
	VGA_BROWN,
	VGA_LIGHT_GRAY,
	VGA_DARK_GRAY,
	VGA_LIGHT_BLUE,
	VGA_LIGHT_GREEN,
	VGA_LIGHT_CYAN,
	VGA_LIGHT_RED,
	VGA_LIGHT_MAGENTA,
	VGA_LIGHT_BROWN,
	VGA_WHITE,
};

void putchar(char c);
void puts(const char *str);
void printf(const char *format, ...);

int gets(char *buf, int size);

void set_foreground_color(enum VGAColor color);
void set_background_color(enum VGAColor color);

#endif
