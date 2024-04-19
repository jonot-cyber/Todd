#include "common.h"
#include "io.h"
#include "mutex.h"
#include "stack_trace.h"
#include "timer.h"

/* Contains the initial state of the ESP register */
u32 initial_esp;

/* The state of the RNG, shared across threads */
static u32 rng_state = 0x12345678;

/* Used to synchronize RNG access */
static mutex_t rng_mutex;

/* Used for sanity checking. If the condition is false, the message is outputted along with a stack trace and a kernel halt. */
void assert(bool condition, const i8 *message) {
#ifdef NDEBUG
	return;
#endif
	if (condition)
		return;
	asm volatile("cli"); // Disable interrupts
	printf("PANIC: %s\n", message);
	stack_trace();
	halt();
}

/* Set an area of memory to all be equal to a byte. This function is optimized for x86 */
void memset(void *start, u8 value, u32 size) {
	/* The number of u32s to fill */
	u32 size_div4 = size >> 2;

	/* Transform 0xAB to 0xABABABAB */
	u32 fill_with = value;
	fill_with |= fill_with << 8;
	fill_with |= fill_with << 16;

	/* Fill in all the u32s */
	for (u32 i = 0; i < size_div4; i++) {
		*((u32 *)start+i) = fill_with;
	}

	/* Fill in the remaining bytes */
	for (u32 i = (size | 3) - 3; i < size; i++) {
		*((u8*)start+i) = value;
	}
}

/* Copy size bytes from src to dst */
void memcpy(const void *src, void *dst, u32 size) {
	if (size == 0)
		return;
	assert(src != NULL, "memcpy: src is null");
	assert(dst != NULL, "memcpy: dst is null");

	/* Cast to u8* to avoid undefined behavior */
	u8 *dst2 = (u8 *)dst;
	u8 *src2 = (u8 *)src;
	for (u32 i = 0; i < size; i++) {
		dst2[i] = src2[i];
	}
}

/* Halt the operating system fully */
void halt() {
	/* Disable interrupts to stop another thread from doing stuff */
	asm volatile("cli");

	/* Infinitely loop */
	while (true);
}

/* Align a value to the nearest 4kb page directory. */
u32 align4k(u32 in) {
	if ((in & 0xFFF) != 0) {
		in &= 0xFFFFF000;
		in += 0x1000;
	}
	return in;
}

/* Sleep for some amount of milliseconds */
void usleep(u32 ms) {
	u32 target = ticks + ms;
	/* Busy waiting for the time target */
	while (ticks <= target);
	return;
}

/* Get a new random variable. Thread safe */
u32 rand() {
	lock(&rng_mutex);
	rng_state ^= rng_state << 13;
	rng_state ^= rng_state >> 17;
	rng_state ^= rng_state << 5;
	unlock(&rng_mutex);
	return rng_state;
}
