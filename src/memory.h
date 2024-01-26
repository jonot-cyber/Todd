#ifndef MEMORY_H
#define MEMORY_H
#include "idt.h"

/**
   Initialize the memory susbsystem
 */
void memory_init(u32);

/**
   Allocate memory in the kernel
 */
void* kmalloc(u32);

/**
   Allocate zeroed-memory in the kernel
 */
void* kmalloc_z(u32);

/**
   Allocate page-aligned memory in the kernel
 */
void* kmallocAligned(u32);

/**
   Allocate memory in the kernel, returning pysical address
 */
void* kmallocPhysical(u32,u32*);

/**
   Allocate page-aligned memory in the kernel, returning physical address
 */
void* kmallocAlignedPhysical(u32, u32*);

/**
   Free memory allocated in the kernel
 */
void kfree(void*);

/**
   Called when you page fault. Don't page fault, its rude >:(
 */
void page_fault(struct Registers);

/**
   Converts a virtual memory address to a physical one. useless right now
 */
u32 virtual_to_physical(u32);

// Paging stuff
struct PageTable {
	u32 entries[1024];
};

struct PageDirectory {
	struct PageTable* entries[1024];
};
#endif
