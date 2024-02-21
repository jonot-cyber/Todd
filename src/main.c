#include "common.h"
#include "gdt.h"
#include "idt.h"
#include "keyboard.h"
#include "memory.h"
#include "monitor.h"
#include "multiboot.h"
#include "parser.h"
#include "scope.h"
#include "task.h"
#include "mutex.h"
#include "timer.h"
#include "exe.h"
#include <string.h>

void outputToddOS() {
	printf("%CrWelcome to %Cb1%CfcT%Cfeo%Cfad%Cfbd%Cf9O%CfdS%Cr\n");
}

void outputHeader() {
	for (int i = 0; i < 80; i++) {
		write_char('=');
	}
	write_char('\n');
}

void outputPassFail(bool value) {
	if (value) {
		printf("%CfaPASS%Cr");
	} else {
		printf("%CfcFAIL%Cr");
	}
}

static i8 buf[256];

void memset_test() {
	// Allocate a 1MB buffer
	i8* buf = kmalloc(1024 * 1024);
	printf("Filling buffer:\n");
	for (u32 i = 0; i < 1024 * 1024; i++) {
		u8 c = i & 0xFF;
		buf[i] = *(i8*)&c;
	}
	printf("Buffer filled\n");
	u32 start_time = ticks;
	for (u32 i = 0; i < 256; i++) {
		memset(buf, i & 0xFF, 1024 * 1024);
	}
	u32 end_time = ticks;
	printf("Finished memset() in %d ms\n", end_time - start_time);
	kfree(buf);
	halt();
}

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
			const i8* toParse = buf;
			struct ParserListContents* lc = parse(&toParse);
			u32 start_time = ticks;
			scope_exec(&scope, lc);
			u32 end_time = ticks;
			printf("%d ms\n", end_time - start_time);
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
	struct MultiBootModule* module_ptr = mboot->mods_addr;
	printf("Module count: %d\n", count);
	for (u32 i = 0; i < count; i++, module_ptr = (struct MultiBootModule*)module_ptr->mod_end + 1) {
		const i8* name = module_ptr->string;
		printf("[%d] %s\n", i + 1, name);
		if (strcmp(name, "initrd.img") != 0) {
			continue;
		}
		for (u32 p = module_ptr->mod_start, c = 0; p != module_ptr->mod_end; p++, c++) {
			printf("0x%x ", *(u32*)p);
			if (c % 4 == 3) {
				usleep(1000);
				write_char('\n');
			}
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
	monitor_init();
	gdt_init();
	idt_init();
	task_init();
	/* Initializing memory will overwrite our module
	 * information. We need to save this somewhere else */
	save_modules(mboot);
	memory_init(mboot->mem_upper * 1024); // mem_upper is in kilobytes, convert to bytes
	timer_init(1000);
	keyboard_init();

	check_modules(mboot);
	u32 pid = fork();
	if (pid) {
		while (true) {
			printf("AAAA\n");
			usleep(1);
		}
	} else {
		while (true) {
			printf("BBBB\n");
			usleep(1);
		}
	}
	halt();

	lisp_repl();

	return 0;
}
