#pragma once

#include "common.h"
#include "ordered_array.h"
#include "memory.h"

namespace Heap {
	const u32 START = 0xC0000000;
	const u32 INITIAL_SIZE = 0x100000;
	const u32 INDEX_SIZE = 0x20000;
	const u32 MAGIC = 0x9970DD05;
	const u32 MIN_SIZE = 0x70000;

	struct Header {
		u32 magic;
		u8 isHole;
		u32 size;
	};

	struct Footer {
		u32 magic;
		Header* header;
	};

	struct Heap {
		OrderedArray::Array index;
		u32 startAddr;
		u32 endAddr;
		u32 maxAddr;
		u8 supervisor;
		u8 readonly;

		static Heap* create(u32, u32, u32, bool, bool);

		void* alloc(u32, bool);
		void free(void*);
	};
};
