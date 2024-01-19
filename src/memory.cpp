#include "memory.h"

extern u32 end;
u32 allocPointer = (u32)&end;

void* kmallocInternal(u32 size, bool align, u32* physical) {
	if (align) {
		allocPointer = align4k(allocPointer);
	}
	if (physical) {
		*physical = allocPointer;
	}
	u32 ret = allocPointer;
	allocPointer += size;
	return (void*)ret;
}

void Memory::init() {
}

void* Memory::kmalloc(u32 size) {
	return kmallocInternal(size, false, nullptr);
}

void* Memory::kmallocAligned(u32 size) {
	return kmallocInternal(size, true, nullptr);
}

void* Memory::kmallocPhysical(u32 size, u32* physical) {
	return kmallocInternal(size, false, physical);
}

void* Memory::kmallocAlignedPhysical(u32 size, u32* physical) {
	return kmallocInternal(size, true, physical);
}

void Memory::kfree(void* ptr) {
	// TODO: Implement
}
