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

void check_modules(struct MultiBoot *mboot) {
	write_string("checking for presence of module information\n");
	bool present = multiboot_mods_present(mboot);
	assert(present, "check_modules: No modules found. Check your qemu flags.");

	u32 count = mboot->mods_count;
	struct MultiBootModule* module_ptr = mboot->mods_addr;
	for (u32 i = 0; i < count; i++, module_ptr = (struct MultiBootModule*)module_ptr->mod_end + 1) {
		assert(module_ptr != NULL, "check_modules: No module\n");
		assert(module_ptr->string != NULL, "check_modules: No module name\n");
		if (strcmp(module_ptr->string, "build/initrd.tar") == 0)
			load_ustar((struct TarHeader*)module_ptr->mod_start);
	}
}

void *save_modules(struct MultiBoot *mboot) {
	assert(multiboot_mods_present(mboot), "save_modules: No module information");
	void *end_ptr = 0;
	u32 count = mboot->mods_count;
	struct MultiBootModule *mod_ptr = mboot->mods_addr;
	for (u32 i = 0; i < count; i++)
		if (mod_ptr->mod_end > (u32)end_ptr)
			end_ptr = (void *)mod_ptr->mod_end;
	return end_ptr;
}

int kmain(struct MultiBoot *mboot, u32 initialStack) {
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

	return 0;
}
