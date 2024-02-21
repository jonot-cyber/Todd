#include "task.h"

#include "common.h"
#include "memory.h"

void* stacks[32];
u32 initial_esps[32];
u32 c_tasks = 1;
u32 i_task = 0;

void task_init() {
	for (u32 i = 0; i < 32; i++) {
		stacks[i] = NULL;
		initial_esps[i] = initial_esp;
	}
}

void* move_stack(void* start, u32 size) {
	assert(size < 4096, "move_stack: Stack Overflow!");
	u16 offset = 4096 - size;
	void* ret = kmalloc(size);
	memcpy(start, ret+offset, size);
	initial_esps[c_tasks] = ((u32)ret + 4096);
	return ret+offset;
}
