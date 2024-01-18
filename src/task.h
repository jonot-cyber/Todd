#pragma once

#include "common.h"
#include "memory.h"

namespace Task {
	struct Task {
		u32 id;
		u32 esp, ebp;
		u32 eip;
		Memory::PageDirectory* pageDirectory;
		Task* next;
	};
	
	void moveStack(void* newStack, u32 size);

	void init();

	void switchTask();

	u32 fork();

	u32 getID();
}
