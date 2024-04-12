#include "syscall.h"

#include "isr.h"
#include "io.h"
#include "task.h"

void syscall_handler(struct Registers regs) {
	switch (regs.eax) {
	case 0: /* write. ebx=ptr, ecx=len */ {
		i8* ptr = (i8*)regs.ebx;
		for (u32 i = 0; i < regs.ecx; i++)
			write_char(ptr[i]);
		break;
	}
	default:
		printf("Syscall 0d%d\n", regs.eax);
		assert(false, "syscall_handler: Not an implemented syscall");
	}
}

void syscall_init() {
	register_interrupt_handler(3, syscall_handler);
}
