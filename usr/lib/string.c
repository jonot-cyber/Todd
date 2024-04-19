#include "string.h"

void memset(void *buf, char c, unsigned size) {
	char *charbuf = buf;
	for (unsigned i = 0; i < size; i++)
		charbuf[i] = c;
}

void memcpy(const void *src, void *dst, unsigned size) {
	const char *charsrc = src;
	char *chardst = dst;
	for (unsigned i = 0; i < size; i++)
		chardst[i] = charsrc[i];
}

int strcmp(const char *a, const char *b) {
	for (;;) {
		if (*a == 0 && *b == 0)
			return 0;
		if (*a == *b) {
			a++;
			b++;
			continue;
		}
		if (*a < *b)
			return -1;
		return 1;
	}
}

unsigned strlen(const char *a) {
	const char *start = a;
	while (*a)
		a++;
	return a - start;
}

unsigned str_to_uint(const char *str) {
	unsigned ret = 0;
	while (*(str++)) {
		ret *= 10;
		ret += (*str - '0');
	}
	return ret;
}
