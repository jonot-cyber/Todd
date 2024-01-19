#pragma once

#include "common.h"

namespace Memory {
	void init();

	void* kmalloc(u32);
	void* kmallocAligned(u32);
	void* kmallocPhysical(u32,u32*);
	void* kmallocAlignedPhysical(u32, u32*);
	void kfree(void*);

	template <typename T>
	T* kmalloc(u32 count = 1) {
		return (T*)kmalloc(sizeof(T) * count);
	}

	template <typename T>
	T* kmallocAligned(u32 count = 1) {
		return (T*)kmallocAligned(sizeof(T) * count);
	}

	template <typename T>
	T* kmallocPhysical(u32 count = 1, u32* physical = nullptr) {
		return (T*)kmallocPhysical(sizeof(T) * count, physical);
	}

	template <typename T>
	T* kmallocAlignedPhysical(u32 count = 1, u32* physical = nullptr) {
		return (T*)kmallocAlignedPhysical(sizeof(T) * count, physical);
	}
};
