#include "common.h"
#include "elf.h"
#include "gdt.h"
#include "heap.h"
#include "idt.h"
#include "io.h"
#include "keyboard.h"
#include "memory.h"
#include "multiboot.h"
#include "string.h"
#include "syscall.h"
#include "tar.h"
#include "task.h"
#include "timer.h"

void output_todd_os() {
	printf("%CrWelcome to %Cb1%CfcT%Cfeo%Cfad%Cfbd%Cf9O%CfdS%Cr\n");
}

void output_header() {
	for (int i = 0; i < 80; i++) {
		write_char('=');
	}
	write_char('\n');
}

void output_pass_fail(bool value) {
	if (value) {
		printf("%CfaPASS%Cr");
	} else {
		printf("%CfcFAIL%Cr");
	}
}

void check_modules(struct MultiBoot* mboot) {
	write_string("checking if presence of module information\n");
	bool present = multiboot_mods_present(mboot);
	if (present) {
		write_string("found\n");
	} else {
		write_string("not found\n");
		return;
	}
	u32 count = mboot->mods_count;
	printf("Module count: %d\n", count);
	struct MultiBootModule* module_ptr = mboot->mods_addr;
	for (u32 i = 0; i < count; i++, module_ptr = (struct MultiBootModule*)module_ptr->mod_end + 1) {
		assert(module_ptr->string != NULL, "check_modules: No module name\n");
		const i8* name = module_ptr->string;
		printf("[%d] %s\n", i + 1, name);
		if (strcmp(name, "build/initrd.tar") == 0) {
			load_ustar((struct TarHeader*)module_ptr->mod_start);
		}
	}
}

void *save_modules(struct MultiBoot* mboot) {
	assert(multiboot_mods_present(mboot), "save_modules: No module information");
	void *end_ptr = 0;
	u32 count = mboot->mods_count;
	struct MultiBootModule* mod_ptr = mboot->mods_addr;
	for (u32 i = 0; i < count; i++) {
		if (mod_ptr->mod_end > end_ptr)
			end_ptr = mod_ptr->mod_end;
	}
	return end_ptr;
}

int kmain(struct MultiBoot* mboot, u32 initialStack) {
	initial_esp = initialStack;
	io_init(true, true);
	gdt_init();
	idt_init();
	task_init();
	
	/* Initializing memory will overwrite our module information. We need to tell the heap allocator not to do this */
	void *end_module_ptr = save_modules(mboot);
	memory_init(end_module_ptr, mboot->mem_upper * 1024); // mem_upper is in kilobytes, convert to bytes
	timer_init(1000);
	keyboard_init();
	syscall_init();

	check_modules(mboot);

	struct FSNode *node = tar_find_file(fs_root, "initrd/lisp.elf");
	elf_load(node->data);
	halt();

	return 0;
}
