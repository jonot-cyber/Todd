#include "common.h"

void memset(u8* start, u8 value, u32 size) {
	for (u32 i = 0; i < size; i++) {
		*(start+i) = value;
	}
}

void halt() {
	while (true); // jail
}

u32 align4k(u32 in) {
	if ((in & 0xFFF) != 0) {
		in &= 0xFFFFF000;
		in += 0x1000;
	}
	return in;
}

bool MultiBoot::memPresent() {
	return flags & (1 << 0);
}

bool MultiBoot::bootDevicePresent() {
	return flags & (1 << 1);
}

bool MultiBoot::cmdLinePresent() {
	return flags & (1 << 2);
}

bool MultiBoot::modsPresent() {
	return flags & (1 << 3);
}
