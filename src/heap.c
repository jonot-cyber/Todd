#include "heap.h"

static struct HeapHeader* heap_start;

void heap_init(void* start_addr, u32 size) {
	heap_start = (struct HeapHeader*)start_addr;

	// Create a heap region that covers all available memory
	struct HeapHeader* full_header = (struct HeapHeader*)start_addr;
	struct HeapFooter* full_footer = (struct HeapFooter*)((u32)start_addr + size - sizeof(struct HeapFooter));
	full_header->is_hole = true;
	full_header->size = size;
	full_footer->header = full_header;
}

void* heap_malloc(u32 size) {
	// Calculate how big of a hole we need, including the header and footer
	u32 full_size = size + sizeof(struct HeapHeader) + sizeof(struct HeapFooter);

	// Loop over all holes
	for (struct HeapHeader* h = (struct HeapHeader*)heap_start; !h->is_last; h = (struct HeapHeader*)((u32)h + h->size)) {
		// Check if memory is available
		if (!h->is_hole) {
			continue;
		}
		// Check if hole is big enough
		if (h->size < full_size) {
			continue;
		}
		// Simple case: If our hole is the exact size we need, fill it
		if (h->size == full_size) {
			h->is_hole = false;
			return (void*)((u32)h + sizeof(struct HeapHeader));
		}

		// If it isn't the exact size, we need to split the hole in two. This code checks if we have room to do that
		u32 remaining_size = h->size - full_size;
		if (remaining_size < sizeof(struct HeapHeader) + sizeof(struct HeapFooter)) {
			continue;
		}

		// Make a new header for our new hole
		struct HeapHeader* next_header = (struct HeapHeader*)((u32)h + full_size);
		next_header->size = remaining_size;
		next_header->is_hole = true;
		next_header->is_last = h->is_last;

		// Modify the existing footer to point to that new header
		struct HeapFooter* next_footer = (struct HeapFooter*)((u32)h + h->size - sizeof(struct HeapFooter));
		next_footer->header = next_header;

		// Make ourselves a new footer.
		struct HeapFooter* new_footer = (struct HeapFooter*)((u32)next_header - sizeof(struct HeapFooter));
		new_footer->header = h;

		// Set up the header correctly
		h->is_hole = false;
		h->is_last = false;
		h->size = full_size;
		return (void*)((u32)h + sizeof(struct HeapHeader));
	}

	// We couldn't find any memory
	return NULL;
}

/**
   Merge a freed hole with a possible hole to the left
 */
bool merge_left(struct HeapHeader* t) {
	// We can't merge left if we are the leftmost heap item
	if (t == heap_start) {
		return false;
	}

	// Get the header to the left of us through the footer
	struct HeapFooter* left_footer = (struct HeapFooter*)((u32)t - sizeof(struct HeapFooter));
	struct HeapHeader* left_header = left_footer->header;

	// If it's a hole, we can merge
	if (!left_header->is_hole) {
		return false;
	}
	left_header->size += t->size;
	left_header->is_last = t->is_last;
	struct HeapFooter* our_footer = (struct HeapFooter*)((u32)t + t->size - sizeof(struct HeapFooter));
	our_footer->header = left_header;
	return true;
}

/**
   Merge a freed hole with a possible hole to the right
 */
bool merge_right(struct HeapHeader* t) {
	if (t->is_last) {
		return false;
	}

	// Get the header and footer to the right of us. If it's a
	// hole we can merge.
	struct HeapHeader* right_header = (struct HeapHeader*)((u32)t + t->size);
	struct HeapFooter* right_footer = (struct HeapFooter*)((u32)right_header + right_header->size - sizeof(struct HeapFooter));
	if (!right_header->is_hole) {
		return false;
	}
	t->size += right_header->size;
	right_footer->header = t;
	return true;
}

/**
   Free a memory region.
 */
void heap_free(void* ptr) {
	struct HeapHeader* h = (struct HeapHeader*)((u32)ptr - sizeof(struct HeapHeader));
	h->is_hole = true;
	// Merge with adjacent holes
	merge_left(h);
	merge_right(h);
}
