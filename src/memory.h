#ifndef MEMORY_H
#define MEMORY_H
#include "idt.h"

extern struct PageDirectory* current_directory;

enum PageTableFlags {
	PAGE_TABLE_PRESENT	 = 0x1,
	PAGE_TABLE_WRITE	 = 0x2,
	PAGE_TABLE_USER		 = 0x4,
	PAGE_TABLE_WRITE_THROUGH = 0x8,
	PAGE_TABLE_DISABLE_CACHE = 0x10,
	PAGE_TABLE_ACCESSED      = 0x20,
};

enum PageFrameFlags {
	PAGE_FRAME_PRESENT	 = 0x1,
	PAGE_FRAME_WRITE	 = 0x2,
	PAGE_FRAME_USER		 = 0x4,
	PAGE_FRAME_WRITE_THROUGH = 0x8,
	PAGE_FRAME_DISABLE_CACHE = 0x10,
	PAGE_FRAME_ACCESSED	 = 0x20,
};

/**
   Initialize the memory susbsystem
 */
void memory_init(void *start, u32 bytes);

/**
   Allocate memory in the kernel
 */
void *kmalloc(u32);

/**
   Allocate zeroed-memory in the kernel
 */
void* kmalloc_z(u32);

/**
   Allocate page-aligned memory in the kernel
 */
void* kmalloc_a(u32);

/**
   Allocate memory in the kernel, returning pysical address
 */
void* kmalloc_p(u32,u32*);

/**
   Allocate page-aligned memory in the kernel, returning physical address
 */
void* kmalloc_ap(u32, u32*);

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

bool alloc_page(struct PageDirectory*, u32);

#endif
