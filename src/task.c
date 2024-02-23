#include "task.h"

#include "common.h"
#include "memory.h"

/* Tasks are stored in a linked list */
static struct Task root_task;
struct Task* current_task;

struct Task* to_del = NULL;

void* stacks[32];
u32 initial_esps[32];

void task_init() {
	current_task = &root_task;
	current_task->stack = NULL;
	current_task->intial_esp = initial_esp;
	current_task->next = current_task;
	current_task->prev = current_task;
}

void move_stack(void* start) {
	struct Task* new_task = kmalloc(sizeof(struct Task));
	void* new_stack = kmalloc(4096);

	u32 size = current_task->intial_esp - (u32)start;
	u16 offset = 4096 - size;
	memcpy(start, new_stack + offset, size);
	
	new_task->intial_esp = (u32)new_stack + 4096;
	void* new_esp = (void*)(new_stack + offset);
	new_task->stack = new_esp;
	new_task->next = current_task->next;
	new_task->prev = current_task;
	current_task->next = new_task;
}

void save_stack(void* esp) {
	current_task->stack = esp;
}

void incr_task() {
	current_task = current_task->next;
}

void* load_stack() {
	return current_task->stack;
}

void delete_task() {
	/* Remove all references to this task */
	struct Task* next = to_del->next;
	struct Task* prev = to_del->prev;
	prev->next = next;
	next->prev = prev;
	if (to_del == &root_task) {
		return;
	}
	void* stack_buffer = (void*)(to_del->intial_esp - 4096);
	kfree(stack_buffer);
	kfree(to_del);
}
