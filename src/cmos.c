#include "cmos.h"
#include "io_port.h"

u8 get_cmos_status() {
	// Select status register
	io_out(0x70, 0x8A);
	u8 reg = io_in(0x71);
	return (reg >> 6) & 1;
}

u8 cmos_read(u8 reg) {
	u8 a = 0;
	u8 b = 1;
	while (get_cmos_status());
	io_out(0x70, 0x80 | reg);
	while (a != b) {
		a = b;
		b = io_in(0x71);
	}
	return b;
}

u8 cmos_seconds() {
	return cmos_read(0x00);
}

u8 cmos_minutes() {
	return cmos_read(0x02);
}

u8 cmos_hours() {
	return cmos_read(0x04);
}

u8 cmos_days() {
	return cmos_read(0x07);
}

u8 cmos_months() {
	return cmos_read(0x08);
}

u16 cmos_years() {
	return (u16)cmos_read(0x09) + (u16)cmos_read(0x32) * 100;
}
