#include "common.h"
#include "monitor.h"

u32 initialEsp;

void assert(bool condition, const i8* message) {
#ifdef NDEBUG
	return;
#endif
	if (condition) {
		return;
	}
	asm volatile("cli"); // Disable interrupts
	Monitor::printf("PANIC: %s\n", message);
	halt();
}


void memset(void* start, u8 value, u32 size) {
	assert(start != nullptr, "memset: Trying to write to nullptr");
	for (u32 i = 0; i < size; i++) {
		*((u8*)start+i) = value;
	}
}

void memcpy(const void* src, void* dst, u32 size) {
	assert(src != nullptr, "memcpy: src is null");
	assert(dst != nullptr, "memcpy: dst is null");
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
//	if ((in & 0xFFFFF000) != 0) {
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