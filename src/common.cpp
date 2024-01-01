#include "common.h"

void memset(void* start, u8 value, u32 size) {
	for (u32 i = 0; i < size; i++) {
		*((u8*)start+i) = value;
	}
}

void memcpy(const void* src, void* dst, u32 size) {
	u8* dst2 = (u8*)dst;
	u8* src2 = (u8*)src;
	for (u32 i = 0; i < size; i++) {
		dst2[i] = src2[i];
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
