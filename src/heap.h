#pragma once

#include "common.h"

namespace Heap {
	struct Header {
		u32 size;
		bool isHole;
		bool isLast;

		bool mergeLeft();
		bool mergeRight();
	};

	struct Footer {
		Header* header;
	};

	void init(void* startAddr, u32 initSize);
	void* malloc(u32 size);
	void free(void* ptr);
};
