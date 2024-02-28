#include "stack_trace.h"

#include "common.h"
#include "io.h"

struct StackFrame {
	struct StackFrame* ebp;
	u32 eip;
};

void stack_trace() {
	write_string("\n===[ Stack Trace ]===\n");
	struct StackFrame* frame;
	asm volatile("movl %%ebp, %0" : "=r"(frame));
	for (u32 i = 1; frame; i++) {
		printf("%d: 0x%x\n", i, frame->eip);
		frame = frame->ebp;
	}
	write_string("=====================\n");
}
