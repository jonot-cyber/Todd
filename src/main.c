#include "common.h"
#include "elf.h"
#include "gdt.h"
#include "heap.h"
#include "idt.h"
#include "io.h"
#include "keyboard.h"
#include "memory.h"
#include "multiboot.h"
#include "lisp/parser.h"
#include "lisp/scope.h"
#include "string.h"
#include "syscall.h"
#include "tar.h"
#include "task.h"
#include "timer.h"
#include "lisp/exe.h"

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

static i8 buf[256];

void lisp_repl() {
	struct Scope scope;
	scope_init(&scope);

	memset(buf, 0, 256);
	u32 buf_i = 0;
	u32 balance = 0;

	write_string("=> ");
	while (true) {
		i8 key = keyboard_scan();
		if (key <= 0) {
			continue;
		}
		if (key >= 'A' && key <= 'Z') {
			continue;
		}
		i8 translated = translate_code(key);
		write_char(translated);
		if (translated == '\n' && balance == 0) {
			buf_i = 0;
			const i8* to_parse = buf;
			u32 start_mem = heap_get_used();
			struct ParserListContents* lc = parse(&to_parse);
			u32 start_time = ticks;
			scope_exec(&scope, lc);
			u32 end_time = ticks;
			u32 end_mem = heap_get_used();
			printf("[%d ms, %d bytes]\n", end_time - start_time, end_mem - start_mem);
			memset(buf, 0, 256);
			write_string("=> ");
		} else if (translated == '\n') {
			write_string(".. ");
		} else if (translated == '\b') {
			if (buf[buf_i - 1] == '(') {
				balance--;
			} else if (buf[buf_i - 1] == ')') {
				balance++;
			}
			buf_i--;
		} else {
			if (translated == '(') {
				balance++;
			}
			if (translated == ')') {
				balance--;
			}
			buf[buf_i++] = translated;
		}
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
		const i8* name = module_ptr->string;
		printf("[%d] %s\n", i + 1, name);
		if (strcmp(name, "build/initrd.tar") == 0) {
			load_ustar((struct TarHeader*)module_ptr->mod_start);
		}
	}
}

void save_modules(struct MultiBoot* mboot) {
	assert(multiboot_mods_present(mboot), "save_modules: No module information");
	u32 count = mboot->mods_count;
	struct MultiBootModule* mod_ptr = mboot->mods_addr;
	for (u32 i = 0; i < count; i++) {
		u32 size = mod_ptr->mod_end - mod_ptr->mod_start;
		void* data = kmalloc(size);
		memcpy((void*)mod_ptr->mod_start, data, size);
		mod_ptr->mod_start = (u32)data;
		mod_ptr->mod_end = (u32)data + size;
		mod_ptr = (struct MultiBootModule*)mod_ptr->mod_end + 1;
	}
}

int kmain(struct MultiBoot* mboot, u32 initialStack) {
	initial_esp = initialStack;
	io_init(true, true);
	gdt_init();
	idt_init();
	task_init();
	
	/* Initializing memory will overwrite our module
	 * information. We need to save this somewhere else */
	save_modules(mboot);
	memory_init(mboot->mem_upper * 1024); // mem_upper is in kilobytes, convert to bytes
	timer_init(1000);
	keyboard_init();
	syscall_init();

	check_modules(mboot);

	struct FSNode *node = tar_find_file(fs_root, "initrd/lisp.elf");
	elf_load(node->data);
	// lisp_repl();

	return 0;
}
