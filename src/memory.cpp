#include "memory.h"

#include "heap.h"
#include "idt.h"
#include "isr.h"
#include "monitor.h"

extern u32 end;
u32 allocPointer = (u32)&end;
bool heapExists = false;

void* kmallocInternal(u32 size, bool align, u32* physical) {
	if (heapExists) {
		assert(!align, "kmallocInternal: heap doesn't support alignment");
		void* tmp = Heap::malloc(size);
		*physical = (u32)tmp;
		return tmp;
	}
	if (align) {
		allocPointer = align4k(allocPointer);
	}
	if (physical) {
		*physical = allocPointer;
	}
	u32 ret = allocPointer;
	allocPointer += size;
	return (void*)ret;
}

void* Memory::kmalloc(u32 size) {
	return kmallocInternal(size, false, nullptr);
}

void* Memory::kmallocAligned(u32 size) {
	return kmallocInternal(size, true, nullptr);
}

void* Memory::kmallocPhysical(u32 size, u32* physical) {
	return kmallocInternal(size, false, physical);
}

void* Memory::kmallocAlignedPhysical(u32 size, u32* physical) {
	return kmallocInternal(size, true, physical);
}

void Memory::kfree(void* ptr) {
	assert(heapExists, "Memory::kfree: Heap not initialized");
	Heap::free(ptr);
}

u32 Memory::virtualToPhysical(u32 virt) {
	PageTable* tables = (PageTable*)0xFFC00000;
	u32 ret = *(u32*)&tables[virt / 0x1000];
	ret &= 0xFFFFF000;
	return ret + virt % 0x1000;
}

void Memory::init(u32 bytes) {
	PageDirectory* pageDirectory = kmallocAligned<PageDirectory>();
	memset(pageDirectory, 0, sizeof(PageDirectory));

	PageTable* tables = kmallocAligned<PageTable>(1024);
	memset(tables, 0, sizeof(PageTable) * 1024);
	for (u32 i = 0; i < 1023; i++) {
		pageDirectory->entries[i] = (PageTable*)((u32)(&tables[i]) | 3);
		for (u32 j = 0; j < 1024; j++) {
			tables[i].entries[j] = (u32)(0x1000 * (1024 * i + j) | 7);
		}
		tables[1023].entries[i] = (u32)(&tables[i]) | 7;
	}
	pageDirectory->entries[1023] = (PageTable*)((u32)(&tables[1023]) | 3);
	tables[1023].entries[1023] = (u32)pageDirectory | 7;

	ISR::registerInterruptHandler(14, Memory::pageFault);

	asm volatile("mov %0, %%cr3" :: "r"(pageDirectory));
	u32 cr0;
	asm volatile("mov %%cr0, %0" : "=r"(cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0" :: "r"(cr0));

	Heap::init((void*)allocPointer, bytes - allocPointer - (0x1000 * 1024));

	heapExists = true;
}

void Memory::pageFault(Registers regs) {
	u32 address;
	asm volatile("mov %%cr2, %0" : "=r"(address));
	Monitor::printf("PAGE FAULT: 0x%x", address);
	halt();
}
