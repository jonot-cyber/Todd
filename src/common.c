#include "common.h"
#include "monitor.h"

u32 initial_esp;

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
	assert(start != NULL, "memset: Trying to write to nullptr");
	for (u32 i = 0; i < size; i++) {
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

bool multiboot_mem_present(struct MultiBoot* m) {
	return m->flags & (1 << 0);
}

bool multiboot_boot_device_present(struct MultiBoot* m) {
	return m->flags & (1 << 1);
}

bool multiboot_cmd_line_present(struct MultiBoot* m) {
	return m->flags & (1 << 2);
}

bool multiboot_mods_present(struct MultiBoot* m) {
	return m->flags & (1 << 3);
}
