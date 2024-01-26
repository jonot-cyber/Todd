#include "memory.h"

#include "heap.h"
#include "idt.h"
#include "isr.h"
#include "monitor.h"

extern u32 end;
u32 alloc_ptr = (u32)&end;
bool heap_exists = false;

void* kmallocInternal(u32 size, bool align, u32* physical) {
	if (heap_exists) {
		assert(!align, "kmallocInternal: heap doesn't support alignment");
		void* tmp = heap_malloc(size);
		*physical = (u32)tmp;
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
	return (void*)ret;
}

void* kmalloc(u32 size) {
	return kmallocInternal(size, false, NULL);
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
	assert(heap_exists, "Memory::kfree: Heap not initialized");
	heap_free(ptr);
}

u32 virtual_to_physical(u32 virt) {
	struct PageTable* tables = (struct PageTable*)0xFFC00000;
	u32 ret = *(u32*)&tables[virt / 0x1000];
	ret &= 0xFFFFF000;
	return ret + virt % 0x1000;
}

void memory_init(u32 bytes) {
	struct PageDirectory* pageDirectory = kmallocAligned(sizeof(struct PageDirectory));
	memset(pageDirectory, 0, sizeof(struct PageDirectory));

	struct PageTable* tables = kmallocAligned(sizeof(struct PageTable) * 1024);
	memset(tables, 0, sizeof(struct PageTable) * 1024);
	for (u32 i = 0; i < 1023; i++) {
		pageDirectory->entries[i] = (struct PageTable*)((u32)(&tables[i]) | 3);
		for (u32 j = 0; j < 1024; j++) {
			tables[i].entries[j] = (u32)(0x1000 * (1024 * i + j) | 7);
		}
		tables[1023].entries[i] = (u32)(&tables[i]) | 7;
	}
	pageDirectory->entries[1023] = (struct PageTable*)((u32)(&tables[1023]) | 3);
	tables[1023].entries[1023] = (u32)pageDirectory | 7;

	register_interrupt_handler(14, page_fault);

	asm volatile("mov %0, %%cr3" :: "r"(pageDirectory));
	u32 cr0;
	asm volatile("mov %%cr0, %0" : "=r"(cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0" :: "r"(cr0));

	heap_init((void*)alloc_ptr, bytes - alloc_ptr - (0x1000 * 1024));

	heap_exists = true;
}

void page_fault(struct Registers regs) {
	u32 address;
	asm volatile("mov %%cr2, %0" : "=r"(address));
	printf("PAGE FAULT: 0x%x", address);
	halt();
}
