#pragma once

#include "common.h"
#include "monitor.h"
#include "heap.h"
#include "isr.h"

namespace Memory {
	// Memory block header
	struct Header {
		u32 magic;
		u8 isHole;
		u32 size;
	};

	struct Footer {
		u32 magic;
		Header* header;
	};
	
	struct Page {
		u32 present	: 1; // If page is present in memory
		u32 rw		: 1; // Read only if 0, R/W if set
		u32 user	: 1; // If 0, superuser only
		u32 accessed	: 1; // If a page has been accessed
		u32 dirty	: 1; // If a page has been written to;
		u32 unused	: 7; // Unused bits
		u32 frame	: 20; // Frame address >> 12;

		void alloc(bool, bool);
		void free();
	};

	struct PageTable {
		Page pages[1024];
	};

	struct PageDirectory {
		// Pointers to page tables
		PageTable* tables[1024];
		// Pointers to page tables with physical address
		u32 tablesPhysical[1024];
		// Physical address of tablesPhysical
		u32 physicalAddr;

		void switchTo();

		Page* getPage(u32, bool);
	};

	void init();

	void pageFault(Registers);
	
	u32 kmallocInternal(u32, bool, u32*);

	u32 kmallocAligned(u32);
	u32 kmallocPhysical(u32, u32*);
	u32 kmallocAlignedPhysical(u32, u32*);
	u32 kmalloc(u32);
	void kfree(void*);
};
