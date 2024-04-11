#include "syscall.h"

#include "isr.h"
#include "io.h"
#include "task.h"

/*
  This is a pretty stupid hack, but it works. Pointers that are stored
  in static variables will be relative to the start of the executable
  section of the elf file, rather than its actual memory
  location. This creates issues, because any pointers passed into a
  systemcall will give the wrong pointer address. We detect this by
  checking if the pointer is within the range of the ELF, and if not,
  transforming it to the correct address.
*/
void* translate_addr(void *addr) {
	/* Check whether the address is outside of the file */
	if (addr < current_task->elf_file_start
	    || addr > current_task->elf_file_start + current_task->elf_file_size) {
		/* Offset the address */
		return addr + (u32)current_task->elf_entry_point;
	} else {
		/* The address is fine and can be returned as is */
		return addr;
	}
}

void syscall_handler(struct Registers regs) {
	switch (regs.eax) {
	case 0: /* write. ebx=ptr, ecx=len */ {
		i8* ptr = translate_addr((i8*)regs.ebx);
		for (u32 i = 0; i < regs.ecx; i++) {
			write_char(ptr[i]);
		}
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
