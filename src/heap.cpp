#include "heap.h"

static Heap::Header* heapStart;

void Heap::init(void* startAddr, u32 size) {
	heapStart = (Header*)startAddr;
	Header* fullHeader = (Header*)startAddr;
	Footer* fullFooter = (Footer*)((u32)startAddr + size - sizeof(Footer));
	fullHeader->isHole = true;
	fullHeader->size = size;
	fullFooter->header = fullHeader;
}
#include "monitor.h"

void* Heap::malloc(u32 size) {
	u32 fullSize = size + sizeof(Header) + sizeof(Footer);
	for (Header* h = (Header*)heapStart; !h->isLast; h = (Header*)((u32)h + h->size)) {
		if (!h->isHole) {
			continue;
		}
		if (h->size < fullSize) {
			continue;
		}
		if (h->size == fullSize) {
			h->isHole = false;
			return (void*)((u32)h + sizeof(Header));
		}

		// Make sure we can split in two
		u32 remainingSize = h->size - fullSize;
		if (remainingSize < sizeof(Header) + sizeof(Footer)) {
			continue;
		}
		Header* nextHeader = (Header*)((u32)h + fullSize);
		nextHeader->size = remainingSize;
		nextHeader->isHole = true;
		nextHeader->isLast = h->isLast;
		Footer* nextFooter = (Footer*)((u32)h + h->size - sizeof(Footer));
		nextFooter->header = nextHeader;
		Footer* newFooter = (Footer*)((u32)nextHeader - sizeof(Footer));
		newFooter->header = h;
		h->isHole = false;
		h->isLast = false;
		h->size = fullSize;
		return (void*)((u32)h + sizeof(Header));
	}
	return nullptr;
}

bool Heap::Header::mergeLeft() {
	// We can't merge left if we are the leftmost heap item
	if (this == heapStart) {
		return false;
	}
	Footer* leftFooter = (Footer*)((u32)this - sizeof(Footer));
	Header* leftHeader = leftFooter->header;
	if (!leftHeader->isHole) {
		return false;
	}
	leftHeader->size += size;
	leftHeader->isLast = isLast;
	Footer* ourFooter = (Footer*)((u32)this + size - sizeof(Footer));
	ourFooter->header = leftHeader;
	return true;
}

bool Heap::Header::mergeRight() {
	if (this->isLast) {
		return false;
	}
	Header* rightHeader = (Header*)((u32)this + size);
	Footer* rightFooter = (Footer*)((u32)rightHeader + rightHeader->size - sizeof(Footer));
	if (!rightHeader->isHole) {
		return false;
	}
	this->size += rightHeader->size;
	rightFooter->header = this;
	return true;
}

void Heap::free(void* ptr) {
	Header* h = (Header*)((u32)ptr - sizeof(Header));
	h->isHole = true;
	h->mergeLeft();
	h->mergeRight();
}
