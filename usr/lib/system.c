#include "system.h"

extern void __sys0(unsigned);
extern void __sys1(unsigned, unsigned);
extern void __sys2(unsigned, unsigned, unsigned);

void write(void *data, unsigned size) {
	__sys2(0, (unsigned)data, size);
}

void read(void *data, unsigned size) {
	__sys2(1, (unsigned)data, size);
}

void *malloc(unsigned size) {
	void *dest = 0;
	__sys2(2, (unsigned)&dest, size);
	return dest;
}

void free(void *ptr) {
	__sys1(3, (unsigned)ptr);
}

void writehex(unsigned num) {
	__sys1(4, num);
}

void *_fopen(const char *name) {
	void *dest = 0;
	__sys2(5, (unsigned)&dest, (unsigned)name);
	return dest;
}

int exec(void *elf) {
	int ret = 0;
	__sys2(6, (unsigned)&ret, (unsigned)elf);
	return ret;
}
