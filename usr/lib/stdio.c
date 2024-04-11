#include "stdio.h"

#include "system.h"

/* TODO: Move this into a separate string.h */
int strlen(const char *str) {
	const char *p = str;
	while (*p) {
		p++;
	}
	return p - str;
}


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

void printf(const char *fmt, ...) {
	void *stack_ptr = (void *)&fmt;
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
				int pow = 1;
				while (pow < val) {
					pow *= 10;
				}
				pow /= 10;
				while (pow) {
					int digit = (val / pow) % 10;
					putchar('0' + digit);
					pow /= 10;
				}
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