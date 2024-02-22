#include "common.h"
#include "io.h"
#include "timer.h"

u32 initial_esp;

static u32 rng_state = 0x12345678;

void assert(bool condition, const i8* message) {
#ifdef NDEBUG
	return;
#endif
	if (condition) {
		return;
	}
	asm volatile("cli"); // Disable interrupts
	printf("PANIC: %s\n", message);
	halt();
}

void memset(void* start, u8 value, u32 size) {
	u32 size_div4 = size >> 2;
	u32 fill_with = value;
	fill_with += fill_with << 8;
	fill_with += fill_with << 16;
	for (u32 i = 0; i < size_div4; i++) {
		*((u32*)start+i) = fill_with;
	}
	for (u32 i = (size | 3) - 3; i < size; i++) {
		*((u8*)start+i) = value;
	}
}

void memcpy(const void* src, void* dst, u32 size) {
	assert(src != NULL, "memcpy: src is null");
	assert(dst != NULL, "memcpy: dst is null");
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

void usleep(u32 ms) {
	u32 target = ticks + ms;
	while (ticks <= target) {
		// Wait
	}
	return;
}

u32 rand() {
	rng_state ^= rng_state << 13;
	rng_state ^= rng_state >> 17;
	rng_state ^= rng_state << 5;
	return rng_state;
}
