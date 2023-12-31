#include "memory.h"

// The kernel's heap
Heap::Heap* kernelHeap = nullptr;

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
	return kmallocInternal(size, false, nullptr);
}

void Memory::kfree(void* ptr) {
	if (kernelHeap == nullptr) {
		// You can't free if the heap hasn't been made yet
		Monitor::writeString("PANIC: Tried to free before heap initialization\n");
		halt();
	}
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
	if (index == (u32)-1) {
		Monitor::writeString("No free frames");
		halt();
	}
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

void Memory::init() {
	u32 memEndPage = 0x1000000; // Assuming physical memory is 16MB

	nFrames = memEndPage / 0x1000;
	frames = (u32*)kmalloc(indexFromBit(nFrames));
	memset((u8*)frames, 0, indexFromBit(nFrames));

	kernelDirectory = (PageDirectory*)kmallocAligned(sizeof(PageDirectory));
	memset((u8*)kernelDirectory, 0, sizeof(PageDirectory));
	currentDirectory = kernelDirectory;

	for (u32 i = Heap::START; i < Heap::START + Heap::INITIAL_SIZE; i += 0x1000) {
		kernelDirectory->getPage(i, true);
	}

	u32 i = 0;
	while (i < placementAddress+0x1000) {
		kernelDirectory->getPage(i, true)->alloc(false, false);
		i += 0x1000;
	}

	for (u32 i = Heap::START; i < Heap::START + Heap::INITIAL_SIZE; i += 0x1000) {
		kernelDirectory->getPage(i, true)->alloc(false, false);
	}

	ISR::registerInterruptHandler(14, pageFault);

	kernelDirectory->switchTo();
	
	kernelHeap = Heap::Heap::create(Heap::START, Heap::START + Heap::INITIAL_SIZE, 0xCFFFF000, false, false);
}

void Memory::PageDirectory::switchTo() {
	currentDirectory = this;
	asm volatile("mov %0, %%cr3":: "r"(&tablesPhysical));
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
		tables[tableIndex] = (PageTable*)kmallocAlignedPhysical(sizeof(PageTable), &tmp);
		memset((u8*)tables[tableIndex], 0, 0x1000);
		tablesPhysical[tableIndex] = tmp | 0x7;
		return &tables[tableIndex]->pages[address % 1024];
	}
	return nullptr;
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
