#pragma GCC push_options
#pragma GCC optimize ("O0")
#include "memory.h"

#include "heap.h"
#include "idt.h"
#include "isr.h"
#include "io.h"
#include "mutex.h"
#include "stack_trace.h"

extern u32 end;

// Make sure that all memory (de)allocations happen atomically.
static mutex_t malloc_mtx;

u32 alloc_ptr = (u32)&end;
bool heap_exists = false;

void* kmallocInternal(u32 size, bool align, u32* physical) {
	lock(&malloc_mtx);
	if (heap_exists) {
		assert(!align, "kmallocInternal: heap doesn't support alignment");
		void* tmp = heap_malloc(size);
		*physical = (u32)tmp;
		unlock(&malloc_mtx);
		return tmp;
	}
	if (align) {
		alloc_ptr = align4k(alloc_ptr);
	}
	if (physical) {
		*physical = alloc_ptr;
	}
	u32 ret = alloc_ptr;
	alloc_ptr += size;
	unlock(&malloc_mtx);
	return (void*)ret;
}

void* kmalloc(u32 size) {
	void* ret = kmallocInternal(size, false, NULL);
	return ret;
}

void* kmalloc_z(u32 size) {
	void* ret = kmalloc(size);
	memset(ret, 0, size);
	return ret;
}

void* kmallocAligned(u32 size) {
	return kmallocInternal(size, true, NULL);
}

void* kmallocPhysical(u32 size, u32* physical) {
	return kmallocInternal(size, false, physical);
}

void* kmallocAlignedPhysical(u32 size, u32* physical) {
	return kmallocInternal(size, true, physical);
}

void kfree(void* ptr) {
	lock(&malloc_mtx);
	assert(heap_exists, "kfree: Heap not initialized");
	heap_free(ptr);
	unlock(&malloc_mtx);
}

u32 virtual_to_physical(u32 virt) {
	struct PageTable* tables = (struct PageTable*)0xFFC00000;
	u32 ret = *(u32*)&tables[virt / 0x1000];
	ret &= 0xFFFFF000;
	return ret + virt % 0x1000;
}

void alloc_page(struct PageDirectory* dir, u32 addr) {
	u32 frame_addr = addr / 0x1000;
	u32 table_i = frame_addr % 1024; /* address in table */
	u32 dir_i = frame_addr / 1024; /* address in directory */
	struct PageTable* table = (struct PageTable*)(((u32)dir->entries[dir_i]) & 0xfffff000);
	if (!table) {
		table = kmallocAligned(sizeof(struct PageTable));
		memset(table, 0, sizeof(struct PageTable));
		u32 table_u = (u32)table;
		table_u |= PAGE_TABLE_PRESENT | PAGE_TABLE_WRITE;
		dir->entries[dir_i] = (struct PageTable*)table_u;
	}
	table->entries[table_i] = addr | PAGE_FRAME_PRESENT | PAGE_FRAME_WRITE | PAGE_FRAME_USER;
}

void memory_init(u32 bytes) {
	const u32 heap_size = 128 * 1024;
	struct PageDirectory* pageDirectory = kmallocAligned(sizeof(struct PageDirectory));
	memset(pageDirectory, 0, sizeof(struct PageDirectory));
	
	u32 it = 0;
	while (it < alloc_ptr + heap_size) {
		alloc_page(pageDirectory, it);
		it += 0x1000;
	}
	register_interrupt_handler(14, page_fault);

	asm volatile("mov %0, %%cr3" :: "r"(pageDirectory));
	u32 cr0;
	asm volatile("mov %%cr0, %0" : "=r"(cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0" :: "r"(cr0));

	heap_init((void*)alloc_ptr, heap_size);

	heap_exists = true;
}

void page_fault(struct Registers regs) {
	u32 address;
	asm volatile("mov %%cr2, %0" : "=r"(address));
	printf("PAGE FAULT: 0x%x", address);
	stack_trace();
	halt();
}

#pragma GCC pop_options
