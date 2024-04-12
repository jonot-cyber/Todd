#include "syscall.h"

#include "isr.h"
#include "io.h"
#include "keyboard.h"

bool is_code_valid(i8 code) {
	if (code <= 0)
		return false;
	if (code >= 'A' && code <= 'Z')
		return false;
	return true;
}

i8 read_char() {
	i8 scan_code = 0;
	while (!is_code_valid(scan_code)) {
		scan_code = keyboard_scan();
	}
	i8 key = translate_code(scan_code);
	write_char(key);
	return key;
}

void syscall_handler(struct Registers regs) {
	switch (regs.eax) {
	case 0: /* write. ebx=ptr, ecx=len */ {
		i8* ptr = (i8*)regs.ebx;
		for (u32 i = 0; i < regs.ecx; i++)
			write_char(ptr[i]);
		break;
	}
	case 1: /* read. ebx=ptr, ecx=len */ {
		i8* ptr = (i8*)regs.ebx;
		for (u32 i = 0; i < regs.ecx; i++) {
			ptr[i] = read_char();
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
