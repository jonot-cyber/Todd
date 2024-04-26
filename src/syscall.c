#include "syscall.h"

#include "elf.h"
#include "isr.h"
#include "io.h"
#include "keyboard.h"
#include "memory.h"
#include "monitor.h"
#include "tar.h"

bool is_code_valid(i8 code) {
	if (code <= 0)
		return false;
	if (code >= 'A' && code <= 'Z')
		return false;
	return true;
}

i8 read_char() {
	i8 scan_code = 0;
	while (!is_code_valid(scan_code))
		scan_code = keyboard_scan();
	i8 key = translate_code(scan_code);
	write_char(key);
	return key;
}

void syscall_handler(struct Registers regs) {
	switch (regs.eax) {
	case SYSCALL_WRITE: /* write. ebx=ptr, ecx=len */ {
		i8 *ptr = (i8 *)regs.ebx;
		for (u32 i = 0; i < regs.ecx; i++)
			write_char(ptr[i]);
		break;
	}
	case SYSCALL_READ: /* read. ebx=ptr, ecx=len */ {
		i8 *ptr = (i8 *)regs.ebx;
		for (u32 i = 0; i < regs.ecx; i++)
			ptr[i] = read_char();
		break;
	}
	case SYSCALL_MALLOC: /* malloc. ebx=ptr to void*, ecx=size */ {
		/* Good operating systems don't do this. Instead, they
		 * expose a way to map pages into your page table, and
		 * leave malloc to userspace code. If you hadn't
		 * noticed, I am not writing a good operating system,
		 * si I will do this anyways */
		void **ptr = (void **)regs.ebx;
		void *ret = kmalloc(regs.ecx);
		*ptr = ret;
		break;
	}
	case SYSCALL_FREE: /* free. ebx=ptr to memory */
		kfree((void *)regs.ebx);
		break;
	case SYSCALL_WRITEHEX:
		printf("0x%x\n", regs.ebx);
		break;
	case SYSCALL_FOPEN: {
		void **ptr = (void **)regs.ebx;
		void *ret = tar_find_file(fs_root, (const i8 *)regs.ecx);
		*ptr = ret;
		break;
	}
	case SYSCALL_EXEC: {
		i32 *ptr = (i32 *)regs.ebx;
		i32 ret = elf_load((void *)regs.ecx);
		*ptr = ret;
		break;
	}
	case SYSCALL_SET_FG: {
		u32 color = regs.ebx;
		if (color > WHITE)
			break;
		foreground_color = color;
		break;
	}
	case SYSCALL_SET_BG: {
		u32 color = regs.ebx;
		if (color > WHITE)
			break;
		background_color = color;
		break;
	}
	case SYSCALL_RAND: {
		u32 *ptr = (u32 *)regs.ebx;
		*ptr = rand();
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
