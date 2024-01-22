#include "cmos.h"
#include "io.h"

u8 getStatus() {
	// Select status register
	IO::out(0x70, 0x8A);
	u8 reg = IO::in(0x71);
	return (reg >> 6) & 1;
}

u8 CMOSRead(u8 reg) {
	u8 a = 0;
	u8 b = 1;
	asm volatile("cli");
	while (getStatus());
	IO::out(0x70, 0x80 | reg);
	while (a != b) {
		a = b;
		b = IO::in(0x71);
	}
	asm volatile("sti");
	return b;
}

u8 CMOS::seconds() {
	return CMOSRead(0x00);
}

u8 CMOS::minutes() {
	return CMOSRead(0x02);
}

u8 CMOS::hours() {
	return CMOSRead(0x04);
}

u8 CMOS::day() {
	return CMOSRead(0x07);
}

u8 CMOS::month() {
	return CMOSRead(0x08);
}

u16 CMOS::year() {
	return (u16)CMOSRead(0x09) + (u16)CMOSRead(0x32) * 100;
}
