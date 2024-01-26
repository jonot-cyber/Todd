#include "gdt.h"

static struct GDTEntry entries[5];

static struct GDTPointer ptr;

void gdt_entry_set(struct GDTEntry* entry, u32 base, u32 limit, u8 a, u8 gran) {
	entry->base_low = (base & 0xFFFF);
	entry->base_middle = (base >> 16) & 0xFF;
	entry->base_high = (base >> 24) & 0xFF;

	entry->limit_low = (limit & 0xFFFF);
	entry->granularity = (limit >> 16) & 0x0F;
	entry->granularity |= gran & 0xF0;
	entry->access = a;
}

void gdt_init() {
	// Set up the pointer data
	ptr.limit = (sizeof(struct GDTEntry) * 5) - 1;
	ptr.base = (u32)&entries;

	// These values can be stolen from
	// https://wiki.osdev.org/GDT_Tutorial. It sets up 5 segments,
	// one for null, and one for each combination of data/code and
	// user/kernel mode. They all cover the entire memory range
	// and do nothing of note. This is because the GDT isn't good
	gdt_entry_set(&entries[0], 0, 0, 0, 0);
	gdt_entry_set(&entries[1], 0, 0xFFFFFFFF, 0x9A, 0xCF);
	gdt_entry_set(&entries[2], 0, 0xFFFFFFFF, 0x92, 0xCF);
	gdt_entry_set(&entries[3], 0, 0xFFFFFFFF, 0xFA, 0xCF);
	gdt_entry_set(&entries[4], 0, 0xFFFFFFFF, 0xF2, 0xCF);

	GDTFlush((u32)&ptr);
}
