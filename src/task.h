#ifndef TASK_H
#define TASK_H

#include "common.h"

extern u32 ret_ptr;
extern u32 jump_to;

u32 fork();
u32 call_cc();

#endif
