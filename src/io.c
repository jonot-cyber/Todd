#include "io.h"

void io_out(u16 port, u8 value) {
	asm volatile("outb %1, %0" : : "dN" (port), "a" (value));
}

u8 io_in(u16 port) {
	u8 value;
	asm volatile("inb %1, %0" : "=a" (value) : "dN" (port));
	return value;
}

u16 io_in16(u16 port) {
	u16 value;
	asm volatile("inw %1, %0" : "=a" (value) : "dN" (port));
	return value;
}
