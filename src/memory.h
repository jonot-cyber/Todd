#ifndef MEMORY_H
#define MEMORY_H
#include "idt.h"

void memory_init(u32);

void* kmalloc(u32);
void* kmallocAligned(u32);
void* kmallocPhysical(u32,u32*);
void* kmallocAlignedPhysical(u32, u32*);
void kfree(void*);

void page_fault(struct Registers);
u32 virtual_to_physical(u32);

// Paging stuff
struct PageTable {
	u32 entries[1024];
};

struct PageDirectory {
	struct PageTable* entries[1024];
};
#endif
