#include "stdio.h"

#include "string.h"
#include "system.h"


void putchar(char c) {
	write(&c, 1);
}

void puts_no_newline(const char* str) {
	unsigned len = (unsigned)strlen(str);
	/* TODO: Fix the signature of the write method to use const */
	write((char *)str, len);
}

void puts(const char *str) {
	puts_no_newline(str);
	putchar('\n');
}

void write_decimal(int num) {
	if (num < 0) {
		putchar('-');
		num *= -1;
	}
	if (num < 10) {
		putchar('0' + num);
		return;
	}
	write_decimal(num / 10);
	write_decimal(num % 10);
}

void printf(const char *fmt, ...) {
	unsigned stack_ptr = (unsigned)&fmt;
	/* Skip the format string in the stack pointer */
	stack_ptr += 4;
	while (*fmt) {
		if (*fmt == '%') {
			char next = *(fmt + 1);
			/* Pick a format string */
			switch (next) {
			case '%':
				putchar('%');
				break;
			case 'd': {
				int val = *(int *)stack_ptr;
				write_decimal(val);
				stack_ptr += 4;
				break;
			}
			case 'c': {
				int val = *(int*)stack_ptr;
				if (val < 0 || val > 255) {
					/* We can't output an invalid character */
					return;
				}
				putchar(val);
				stack_ptr += 4;
				break;
			}
			case 's': {
				const char *str = *(const char **)stack_ptr;
				puts_no_newline(str);
				stack_ptr += 4;
				break;
			}
			}
			fmt += 2;
		} else {
			putchar(*fmt);
			fmt++;
		}
	}
}

/* This funciton is safer than the normal gets */
int gets(char *buf, int size) {
	for (int i = 0; i < size - 1; i++) {
		char ch;
		read(&ch, 1);
		if (ch == '\n') {
			buf[i] = '\0';
			return i;
		}
		else
			buf[i] = ch;
	}
	/* make sure that the string is always null terminated */
	buf[size - 1] = '\0';
	return size - 1;
}

void set_foreground_color(enum VGAColor color) {
	setfg(color);
}

void set_background_color(enum VGAColor color) {
	setbg(color);
}
