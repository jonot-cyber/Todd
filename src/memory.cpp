#include "memory.h"

#include "heap.h"
#include "isr.h"
#include "monitor.h"

// The kernel's heap
static Heap::Heap* kernelHeap = nullptr;

extern u32 end;
u32 placementAddress = (u32)&end;

Memory::PageDirectory* kernelDirectory = nullptr;
Memory::PageDirectory* currentDirectory = nullptr;

// Internal kmalloc function
u32 Memory::kmallocInternal(u32 size, bool align, u32* physical) {
	if (kernelHeap != nullptr) {
		// If the heap has been set up, route malloc through it
		void* address = kernelHeap->alloc(size, align);
		if (physical != nullptr) {
			Page* page = kernelDirectory->getPage((u32)address, false);
			*physical = page->frame * 0x1000 + ((u32)address & 0xFFF);
		}
		return (u32)address;
	}
	if (align) { // If the address isn't page aligned, align it
		placementAddress = align4k(placementAddress);
	}
	if (physical) {
		*physical = placementAddress;
	}
	u32 tmp = placementAddress;
	placementAddress += size;
	return tmp;
}

u32 Memory::kmallocAligned(u32 size) {
	return kmallocInternal(size, true, nullptr);
}

u32 Memory::kmallocPhysical(u32 size, u32* physical) {
	return kmallocInternal(size, false, physical);
}

u32 Memory::kmallocAlignedPhysical(u32 size, u32* physical) {
	return kmallocInternal(size, true, physical);
}

u32 Memory::kmalloc(u32 size) {
	u32 ret = kmallocInternal(size, false, nullptr);
	return ret;
}

void Memory::kfree(void* ptr) {
	assert(kernelHeap, "Memory::kfree: tried to free without heap");
	kernelHeap->free(ptr);
}

// Paging stuff
u32* frames;
u32 nFrames;

u32 indexFromBit(u32 a) {
	return a / 32;
}

u32 offsetFromBit(u32 a) {
	return a % 32;
}

static void setFrame(u32 frameAddr) {
	u32 frame = frameAddr / 0x1000;
	u32 index = indexFromBit(frame);
	u32 offset = offsetFromBit(frame);
	frames[index] |= (1 << offset);
}

// Clear a bit from the frames
static void clearFrame(u32 frameAddr) {
	u32 frame = frameAddr / 0x1000;
	u32 index = indexFromBit(frame);
	u32 offset = offsetFromBit(frame);
	frames[index] &= ~(1 << offset);
}

// Find the first free frame
static u32 firstFrame() {
	for (u32 i = 0; i < indexFromBit(nFrames); i++) {
		if (frames[i] == 0xFFFFFFFF) { // No free frames here
			continue;
		}
		for (u32 j = 0; j < 32; j++) {
			u32 toTest = 1 << j;
			if (!(frames[i] & toTest)) {
				return i * 32 + j;
			}
		}
	}
	return -1;
}

void Memory::Page::alloc(bool isKernel, bool isWriteable) {
	if (frame != 0) { // Page already allocated
		return;
	}
	u32 index = firstFrame();
	assert(index != (u32)-1, "Memory::Page::alloc: No free frames");
	setFrame(index * 0x1000);
	present = true;
	rw = isWriteable;
	user = !isKernel;
	frame = index;
}

void Memory::Page::free() {
	if (frame == 0) {
		return;
	}
	clearFrame(frame);
	frame = 0;
}

void Memory::Page::copyPhysical(Memory::Page* other) {
	copyPagePhysical(frame*0x1000, other->frame*0x1000);
}

Memory::PageTable* Memory::PageTable::clone(u32* physicalAddress) {
	Memory::PageTable* table = Memory::kmallocAlignedPhysical<Memory::PageTable>(1, physicalAddress);
	memset(table, 0, sizeof(Memory::PageTable));

	for (u32 i = 0; i < 1024; i++) {
		if (!pages[i].frame) {
			continue;
		}
		table->pages[i].alloc(false, false);
		if (pages[i].present) table->pages[i].present = 1;
		if (pages[i].rw) table->pages[i].rw = 1;
		if (pages[i].user) table->pages[i].user = 1;
		if (pages[i].accessed) table->pages[i].accessed = 1;
		if (pages[i].dirty) table->pages[i].dirty = 1;
		pages[i].copyPhysical(&table->pages[i]);
	}
	return table;
}

void Memory::init(u32 bytes) {
	nFrames = 0x1000000 / 0x1000;
	frames = kmalloc<u32>(indexFromBit(nFrames) / sizeof(u32));
	memset(frames, 0, indexFromBit(nFrames));

	kernelDirectory = kmallocAligned<PageDirectory>();
	memset(kernelDirectory, 0, sizeof(PageDirectory));
	kernelDirectory->physicalAddr = (u32)kernelDirectory->tablesPhysical;

	for (u32 i = Heap::START; i < Heap::START + Heap::INITIAL_SIZE; i += 0x1000) {
		kernelDirectory->getPage(i, true);
	}

	for (u32 i = 0; i < placementAddress+0x1000; i += 0x1000) {
		kernelDirectory->getPage(i, true)->alloc(false, false);
	}

	for (u32 i = Heap::START; i < Heap::START + Heap::INITIAL_SIZE; i += 0x1000) {
		kernelDirectory->getPage(i, true)->alloc(false, false);
	}

	ISR::registerInterruptHandler(14, pageFault);

	kernelDirectory->switchTo();

	kernelHeap = Heap::Heap::create(Heap::START, Heap::START + Heap::INITIAL_SIZE, 0xCFFFF000, false, false);

	currentDirectory = kernelDirectory;
//	currentDirectory = kernelDirectory->clone();
//	currentDirectory->physicalAddr = kernelDirectory->physicalAddr;
//	currentDirectory->switchTo();
}

void Memory::PageDirectory::switchTo() {
	currentDirectory = this;
	asm volatile("mov %0, %%cr3":: "r"(physicalAddr));
	u32 cr0;
	asm volatile("mov %%cr0, %0":"=r"(cr0));
	// Enable paging;
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0":: "r"(cr0));
}

Memory::Page* Memory::PageDirectory::getPage(u32 address, bool make) {
	address /= 0x1000;
	u32 tableIndex = address / 1024;
	if (tables[tableIndex]) { // If table is assigned
		return &tables[tableIndex]->pages[address % 1024];
	}
	if (make) {
		u32 tmp;
		tables[tableIndex] = kmallocAlignedPhysical<PageTable>(1, &tmp);
		memset(tables[tableIndex], 0, 0x1000);
		tablesPhysical[tableIndex] = tmp | 0x7;
		return &tables[tableIndex]->pages[address % 1024];
	}
	return nullptr;
}

Memory::PageDirectory* Memory::PageDirectory::clone() {
	u32 physical;
	Memory::PageDirectory* ret = Memory::kmallocAlignedPhysical<PageDirectory>(1, &physical);
	memset(ret, 0, sizeof(Memory::PageDirectory));

	// Get offset of tables from start
	u32 offset = (u32)ret->tablesPhysical - (u32)ret;
	ret->physicalAddr = physical + offset;

	for (u32 i = 0; i < 1024; i++) {
		if (!tables[i]) {
			continue;
		}

		if (kernelDirectory->tables[i] == tables[i]) {
			// In the kernel, use pointer
			ret->tables[i] = tables[i];
			ret->tablesPhysical[i] = tablesPhysical[i];
		} else {
			u32 p2;
			ret->tables[i] = tables[i]->clone(&p2);
			ret->tablesPhysical[i] = p2 | 0x07;
		}
	}
	return ret;
}

void Memory::pageFault(Registers registers) {
	u32 faultAddr;
	asm volatile("mov %%cr2, %0": "=r"(faultAddr));

	bool present = !(registers.errCode & 0x1); // Page not present
	int rw = registers.errCode & 0x2;
	int user = registers.errCode & 0x4;
	int reserved = registers.errCode & 0x8;
	// int instructionFetch = registers.errCode & 0x10;

	Monitor::writeString("\n==[ Page Fault! ]==\n");
	if (present) Monitor::writeString("present\n");
	if (rw) Monitor::writeString("read-only\n");
	if (user) Monitor::writeString("user-mode\n");
	if (reserved) Monitor::writeString("reserved\n");
	Monitor::printf("Location: 0x%x\n", faultAddr);
	halt();
}
