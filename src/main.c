#include "common.h"
#include "gdt.h"
#include "idt.h"
#include "keyboard.h"
#include "memory.h"
#include "monitor.h"
#include "parser.h"
#include "timer.h"
#include "exe.h"

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

void multiboot_info(struct MultiBoot* mboot) {
	printf("Flags: %b\n", mboot->flags);
	write_string("Memory Information: ");
	outputPassFail(multiboot_mem_present(mboot));
	if (multiboot_mem_present(mboot)) {
		printf("\n| Lower Memory Limit: %dKB\n", mboot->mem_lower);
		printf("| Upper Memory Limit: %dKB\n", mboot->mem_upper);
	}
	write_string("Boot Device: ");
	outputPassFail(multiboot_boot_device_present(mboot));
	if (multiboot_boot_device_present(mboot)) {
		printf("\n| Boot Device: 0x%x", mboot->boot_device);
	}
	write_string("\nCMD Line: ");
	outputPassFail(multiboot_cmd_line_present(mboot));
	if (multiboot_cmd_line_present(mboot)) {
		printf("\n| Command Line: %s", mboot->cmd_line);
	}
	write_string("\nBoot Modules: ");
	outputPassFail(multiboot_boot_device_present(mboot));
	if (multiboot_boot_device_present(mboot)) {
		printf("\n| #: %d", mboot->mods_count);
		assert(mboot->mods_count == 0, "multiboot_info: Modules not supported");
	}
	write_char('\n');
}

static i8 buf[256];

#include "scope.h"
int kmain(struct MultiBoot* mboot, u32 initialStack) {
	initial_esp = initialStack;
	gdt_init();
	idt_init();
	monitor_init();
	timer_init(1000);
	memory_init(mboot->mem_upper * 1024);
	keyboard_init();

	struct Scope scope;
	scope_init(&scope);

	memset(buf, 0, 256);
	u32 bufI = 0;
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
			bufI = 0;
			const i8* toParse = buf;
			struct ParserListContents* lc = parse(&toParse);
			scope_exec(&scope, lc);
			memset(buf, 0, 256);
			write_string("=> ");
		} else if (translated == '\n') {
			write_string(".. ");
		} else if (translated == '\b') {
			if (buf[bufI - 1] == '(') {
				balance--;
			} else if (buf[bufI - 1] == ')') {
				balance++;
			}
			bufI--;
		} else {
			if (translated == '(') {
				balance++;
			}
			if (translated == ')') {
				balance--;
			}
			buf[bufI++] = translated;
		}
	}
	halt();
	return 0;
}
