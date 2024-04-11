#ifndef TASK_H
#define TASK_H

#include "common.h"

struct Task {
	void* stack;
	u32 intial_esp;
	struct Task* next;
	struct Task* prev;
	u32 priority;
	u32 age;
	/* The entry point of the currently running executable, if it exists. This is needed for a hack in syscall.c  */
	void *elf_entry_point;
	void *elf_file_start;
	u32 elf_file_size;
};

extern struct Task* current_task;
extern struct Task* to_del;

void task_init();

u32 fork();
u32 switch_task();
u32 join();

void move_stack(void* start);
void save_stack(void* esp);
void incr_task();
void* load_stack();
void delete_task();

#endif
