#include "memory.h"
#include "idt.h"
#include "isr.h"
#include "monitor.h"

extern u32 end;
u32 allocPointer = (u32)&end;

void* kmallocInternal(u32 size, bool align, u32* physical) {
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
	// TODO: Implement
}

void Memory::init() {
	PageDirectory* pageDirectory = kmallocAligned<PageDirectory>();
	memset(pageDirectory, 0, sizeof(PageDirectory));

	PageTable* pTable1 = kmallocAligned<PageTable>();
	memset(pTable1, 0, sizeof(PageTable));
	pageDirectory->entries[0] = (PageTable*)((u32)pTable1 | 3);

	u32 i = 0;
	while (i < allocPointer + 0x1000) {
		u32 addr = i / 0x1000;
		u32 iTable = addr / 1024;
		pTable1->entries[addr] = i | 7;
		i += 0x1000;
	};

	PageTable* pLast = kmallocAligned<PageTable>();
	memset(pLast, 0, sizeof(PageTable));
	pageDirectory->entries[1023] = (PageTable*)((u32)pLast | 3);
	pLast->entries[1023] = (u32)pageDirectory | 7;
	pLast->entries[0] = (u32)pTable1 | 7;

	ISR::registerInterruptHandler(14, Memory::pageFault);

	asm volatile("mov %0, %%cr3" :: "r"(pageDirectory));
	u32 cr0;
	asm volatile("mov %%cr0, %0" : "=r"(cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0" :: "r"(cr0));
}

void Memory::pageFault(Registers regs) {
	u32 address;
	asm volatile("mov %%cr2, %0" : "=r"(address));
	Monitor::printf("PAGE FAULT: 0x%x", address);
	halt();
}
