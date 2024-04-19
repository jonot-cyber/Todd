#include "multiboot.h"

bool multiboot_mem_present(struct MultiBoot *m) {
	return m->flags & (1 << 0);
}

bool multiboot_boot_device_present(struct MultiBoot *m) {
	return m->flags & (1 << 1);
}

bool multiboot_cmd_line_present(struct MultiBoot *m) {
	return m->flags & (1 << 2);
}

bool multiboot_mods_present(struct MultiBoot *m) {
	return m->flags & (1 << 3);
}
