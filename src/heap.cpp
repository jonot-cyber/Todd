#include "heap.h"
#include "memory.h"

extern Memory::PageDirectory kernelDirectory;

u32 Heap::Heap::findSmallestHole(u32 size, bool pageAlign) {
	u32 i;
	for (i = 0; i < index.size; i++) {
		// Find the current header
		Header* header = index[i];
		u32 holeSize = header->size;
		if (pageAlign) {
			// Account for alignment
			holeSize -= 0x1000 - ((u32)header + sizeof(Header)) % 0x1000;
		} else if (holeSize >= size) {
			break;
		}
	}
	if (i == index.size) {
		// We didn't find any holes
		return -1;
	}
	return i;
}

u32 Heap::Heap::size() {
	return endAddr - startAddr;
}

static i8 headerLessThan(Heap::Header* a, Heap::Header* b) {
	return a->size < b->size;
}

Heap::Heap* Heap::Heap::create(u32 start, u32 end, u32 max, bool supervisor, bool readOnly) {
	using namespace Heap;
	// Allocate memory for the heap and create the index array
	Heap* heap = Memory::kmalloc<Heap>();
	heap->index = OrderedArray::Array<Header*>::place((void*)start, INDEX_SIZE, &headerLessThan);

	// Offset the start to make room for the index array
	start += sizeof(Header*) * INDEX_SIZE;
	
	// If start isn't aligned, align it
	start = align4k(start);
	
	// Setup heap structure
	heap->startAddr = start;
	heap->endAddr = end;
	heap->maxAddr = max;
	heap->supervisor = supervisor;
	heap->readonly = readOnly;

	// Create a header for a hole that takes up the whole space.
	Header* hole = (Header*)start;
	hole->size = end - start;
	hole->magic = MAGIC;
	hole->isHole = true;
	heap->index.insert(hole);

	return heap;
}

void Heap::Heap::expand(u32 newSize) {
	// Make sure that the new size is aligned
	if ((newSize & 0xFFFFF000) != 0) {
		newSize &= 0xFFFFF000;
		newSize += 0x1000;
	}
	newSize = align4k(newSize);
	u32 oldSize = size();
	for (u32 i = oldSize; i < newSize; i += 0x1000) {
		Memory::Page* page = kernelDirectory.getPage(startAddr + i, true);
		page->alloc(supervisor, readonly);
	}
	endAddr = startAddr + newSize;
}

u32 Heap::Heap::contract(u32 newSize) {
	if (newSize >= size()) {
		Monitor::writeString("PANIC: Cannot contract to smaller size\n");
		halt();
	}
	newSize = align4k(newSize);
	if (newSize < MIN_SIZE) {
		newSize = MIN_SIZE;
	}

	// Free now unused pages
	u32 oldSize = size();
	for (u32 i = newSize; i < oldSize; i += 0x1000) {
		kernelDirectory.getPage(startAddr + i, false)->free();
	}
	
	endAddr = startAddr + newSize;
	return newSize;
}

void* Heap::Heap::alloc(u32 size, bool pageAlign) {
	u32 newSize = size + sizeof(Header) + sizeof(Footer);
	i32 iterator = findSmallestHole(newSize, pageAlign);
	// If there's no space, expand the heap and make some
	if (iterator == -1) {
		u32 oldLength = this->size();
		u32 oldEndAddr = endAddr;
	
		expand(oldLength + newSize);
		u32 newLength = this->size();

		// Find the last header in memory
		i32 idx = -1;
		u32 value = 0;
		for (u32 i = 0; i < index.size; i++) {
			u32 tmp = (u32)index[i];
			if (tmp > value) {
				value = tmp;
				idx = i;
			}
		}
		// If there are no headers in the heap, make
		// one. Otherwise, fill the last hole
		Header* header = nullptr;
		if (idx == -1) {
			header = (Header*)oldEndAddr;
		} else {
			header = index[idx];
		}
		
		header->magic = MAGIC;
		header->size = newLength - oldLength;
		header->isHole = true;
		Footer* footer = (Footer*)(oldEndAddr + header->size - sizeof(Footer));
		footer->magic = MAGIC;
		footer->header = header;
		index.insert(header);

		// Try again with the new space	      
		return alloc(size, pageAlign);
	}

	Header* originalHoleHeader = index[iterator];
	u32 originalHolePosition = (u32)originalHoleHeader;
	u32 originalHoleSize = originalHoleHeader->size;

	if (originalHoleSize - newSize < sizeof(Header) + sizeof(Footer)) {
		size += originalHoleSize - newSize;
		newSize = originalHoleSize;
	}

	// If we need to page align, create a new hole. Otherwise, the hole is done
	if (pageAlign && originalHolePosition & 0xFFF) {
		u32 newLocation = originalHolePosition + 0x1000 - (originalHolePosition & 0xFFF) - sizeof(Header);
		Header* holeHeader = (Header*)originalHolePosition;
		holeHeader->size = 0x1000 - (originalHolePosition & 0xFFF) - sizeof(Header);
		holeHeader->magic = MAGIC;
		holeHeader->isHole = true;
		Footer* holeFooter = (Footer*)((u32)newLocation - sizeof(Footer));
		holeFooter->magic = MAGIC;
		holeFooter->header = holeHeader;
		originalHolePosition = newLocation;
		originalHoleSize = originalHoleSize - holeHeader->size;
	} else {
		index.remove(iterator);
	}

	// Make a new header and footer
	Header* blockHeader = (Header*)originalHolePosition;
	blockHeader->magic = MAGIC;
	blockHeader->isHole = false;
	blockHeader->size = newSize;
	Footer* blockFooter = (Footer*)(originalHolePosition + sizeof(Header) + size);
	blockFooter->magic = MAGIC;
	blockFooter->header = blockHeader;

	if (originalHoleSize - newSize > 0) {
		Header* holeHeader = (Header*)(originalHolePosition + sizeof(Header) + size + sizeof(Footer));
		holeHeader->magic = MAGIC;
		holeHeader->isHole = true;
		holeHeader->size = originalHoleSize - newSize;
		Footer* holeFooter = (Footer*)((u32)holeHeader + originalHoleSize - newSize - sizeof(Footer));
		if ((u32)holeFooter < endAddr) {
			holeFooter->magic = MAGIC;
			holeFooter->header = holeHeader;
		}
		index.insert(holeHeader);
	}

	return (void*)((u32)blockHeader+sizeof(Header));
}

void Heap::Heap::free(void* p) {
	// We can't free a null pointer
	if (p == nullptr) {
		return;
	}

	// Get the header and the footer
	Header* header = (Header*)((u32)p - sizeof(Header));
	Footer* footer = (Footer*)((u32)header + header->size - sizeof(Footer));
	// Make it a hole
	header->isHole = true;
	u8 doAdd = true;

	// Unify left
	Footer* testFooter = (Footer*)((u32)header - sizeof(Footer));
	if (testFooter->magic == MAGIC && testFooter->header->isHole) {
		u32 cacheSize = header->size;
		header = testFooter->header;
		footer->header = header;
		header->size += cacheSize;
		doAdd = false;
	}

	// Unify right
	Header* testHeader = (Header*)((u32)footer + sizeof(Footer));
	if (testHeader->magic == MAGIC && testHeader->isHole) {
		header->size += testHeader->size;
		testFooter = (Footer*)((u32)testHeader + testHeader->size - sizeof(Footer));
		footer = testFooter;
		u32 i = 0;
		while ((i < index.size) && (index[i] != testHeader)) {
			i++;
		}
		index.remove(i);
	}

	if ((u32)footer + sizeof(Footer) == endAddr) {
		u32 oldLength = size();
		u32 newLength = contract((u32)header - startAddr);
		if (header->size - (oldLength - newLength) > 0) {
			header->size -= oldLength - newLength;
			footer = (Footer*)((u32)header + header->size - sizeof(Footer));
			footer->magic = MAGIC;
			footer->header = header;
		} else {
			u32 i = 0;
			while ((i < index.size)
			       && (index[i] != testHeader)) {
				i++;
			}
			if (i < index.size) {
				index.remove(i);
			}
		}
	}

	if (doAdd) {
		index.insert(header);
	}
}
