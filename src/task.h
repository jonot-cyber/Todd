#ifndef TASK_H
#define TASK_H

#include "common.h"

extern void* stacks[32];
extern u32 initial_esps[32];
extern u32 c_tasks;
extern u32 i_task;

void task_init();
u32 fork();
u32 switch_task();
void move_stack(void* start);

#endif
