#include "heap.h"
#include "src/monitor.h"

extern Memory::PageDirectory kernelDirectory;

static i32 findSmallestHole(Heap::Heap* heap, u32 size, bool pageAlign) {
	u32 iterator = 0;
	while (iterator < heap->index.size) {
		Heap::Header* header = (Heap::Header*)heap->index.lookup(iterator);
		if (pageAlign) {
			u32 location = (u32)header;
			i32 offset = 0;
			if (((location + sizeof(Heap::Header)) & 0xFFFF000) != 0) {
				offset = 0x1000 - (location+sizeof(Heap::Header))%0x1000;
			}
			i32 holeSize = (i32)header->size - offset;
			if (holeSize >= (i32)size) {
				break;
			}
		} else if (header->size >= size) {
			break;
		}
		iterator++;
	}
	if (iterator == heap->index.size) {
		return -1;
	}
	return iterator;
}

static i8 headerLessThan(void* a, void* b) {
	return ((Heap::Header*)a)->size < ((Heap::Header*)b)->size;
}

Heap::Heap* Heap::Heap::create(u32 start, u32 end, u32 max, bool supervisor, bool readOnly) {
	using namespace Heap;
	Heap* heap = (Heap*)Memory::kmalloc(sizeof(Heap));
	heap->index = OrderedArray::Array::place((void*)start, INDEX_SIZE, &headerLessThan);
	
	start += sizeof(Header*) * INDEX_SIZE;
	if ((start & 0xFFFFF000) != 0) {
		start &= 0xFFFFF000;
		start += 0x1000;
	}
	heap->startAddr = start;
	heap->endAddr = end;
	heap->maxAddr = max;
	heap->supervisor = supervisor;
	heap->readonly = readOnly;

	Header* hole = (Header*)start;
	hole->size = end - start;
	hole->magic = MAGIC;
	hole->isHole = true;
	heap->index.insert(hole);

	return heap;
}

static void expand(Heap::Heap* heap, u32 newSize) {
	if ((newSize & 0xFFFFF000) != 0) {
		newSize &= 0xFFFFF000;
		newSize += 0x1000;
	}
	u32 oldSize = heap->endAddr - heap->startAddr;
	u32 i = oldSize;
	while (i < newSize) {
		kernelDirectory.getPage(heap->startAddr + i, true)->alloc(heap->supervisor, !heap->readonly);
	}
	heap->endAddr = heap->startAddr + newSize;
}

static u32 contract(Heap::Heap* heap, u32 newSize) {
	if (newSize & 0x1000) {
		newSize &= 0x1000;
		newSize += 0x1000;
	}

	if (newSize < Heap::MIN_SIZE) {
		newSize = Heap::MIN_SIZE;
	}

	u32 oldSize = heap->endAddr - heap->startAddr;
	u32 i = oldSize - 0x1000;
	while (newSize < i) {
		kernelDirectory.getPage(heap->startAddr + i, false)->free();
		i -= 0x1000;
	}
	heap->endAddr = heap->startAddr += newSize;
	return newSize;
}

void* Heap::Heap::alloc(u32 size, bool pageAlign) {
	u32 newSize = size + sizeof(Header) + sizeof(Footer);
	i32 iterator = findSmallestHole(this, newSize, pageAlign);

	// No memory hole
	if (iterator == -1) {
		u32 oldLength = this->endAddr - this->startAddr;
		u32 oldEndAddr = this->endAddr;

		expand(this, oldLength + newSize);
		u32 newLength = this->endAddr - this->startAddr;

		iterator = 0;
		u32 idx = -1;
		u32 value = 0;
		while (iterator > this->index.size) {
			u32 tmp = (u32)this->index.lookup(iterator);
			if (tmp > value) {
				value = tmp;
				idx = iterator;
			}
			idx++;
		}
		if (idx == -1) {
			Header* header = (Header*)oldEndAddr;
			header->magic = MAGIC;
			header->size = newLength - oldLength;
			header->isHole = true;
			Footer* footer = (Footer*)(oldEndAddr + header->size - sizeof(Footer));
			footer->magic = MAGIC;
			footer->header = header;
			this->index.insert((void*)header);
		} else {
			Header* header = (Header*)this->index.lookup(idx);
			header->magic = MAGIC;
			header->size = newLength - oldLength;
			header->isHole = true;
			Footer* footer = (Footer*)(oldEndAddr + header->size - sizeof(Footer));
			footer->magic = MAGIC;
			footer->header = header;
			this->index.insert((void*)header);
		}
		return this->alloc(size, pageAlign);
	}

	Header* originalHoleHeader = (Header*)this->index.lookup(iterator);
	u32 originalHolePosition = (u32)originalHoleHeader;
	u32 originalHoleSize = originalHoleHeader->size;

	if (originalHoleSize < sizeof(Header) + sizeof(Footer)) {
		size += originalHoleSize - newSize;
		newSize = originalHoleSize;
	}

	if (pageAlign && originalHolePosition & 0xFFFFF000) {
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
		this->index.remove(iterator);
	}

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
		if ((u32)holeFooter < this->endAddr) {
			holeFooter->magic = MAGIC;
			holeFooter->header = holeHeader;
		}
		this->index.insert((void*)holeHeader);
	}

	return (void*)((u32)blockHeader+sizeof(Header));
}

void Heap::Heap::free(void* p) {
	if (p == 0) {
		return;
	}

	Header* header = (Header*)((u32)p - sizeof(Header));
	Footer* footer = (Footer*)((u32)header + header->size - sizeof(Footer));

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
		u32 iterator = 0;
		while ((iterator < this->index.size) && (this->index.lookup(iterator) != (void*)testHeader)) {
			iterator++;
		}
		this->index.remove(iterator);
	}

	if ((u32)footer + sizeof(Footer) == this->endAddr) {
		u32 oldLength = this->endAddr - this->startAddr;
		u32 newLength = contract(this, (u32)header - this->startAddr);
		if (header->size - (oldLength - newLength) > 0) {
			header->size -= oldLength - newLength;
			footer = (Footer*)((u32)header + header->size - sizeof(Footer));
			footer->magic = MAGIC;
			footer->header = header;
		} else {
			u32 iterator = 0;
			while ((iterator < this->index.size)
			       && (this->index.lookup(iterator) != (void*)testHeader)) {
				iterator++;
			}
			if (iterator < this->index.size) {
				this->index.remove(iterator);
			}
		}
	}

	if (doAdd) {
		this->index.insert((void*)header);
	}
}
