#include "heap.h"

#include "memory.h"
#include "task.h"

static u32 used_bytes = 0;

static struct HeapHeader* heap_start;

void heap_init(void* start_addr, u32 size) {
	// Keeps track of how many bytes have been used
	used_bytes = 0;
	heap_start = (struct HeapHeader*)start_addr;

	// Create a heap region that covers all available memory
	struct HeapHeader* full_header = (struct HeapHeader*)start_addr;
	struct HeapFooter* full_footer = (struct HeapFooter*)((u32)start_addr + size - sizeof(struct HeapFooter) - sizeof(struct HeapHeader));
	full_header->is_hole = true;
	full_header->size = size;
	full_footer->header = full_header;
	full_header->is_last = true;
}

bool could_fit_page_table(struct HeapHeader* h, u32 to_alloc) {
	u32 new_start_point = (u32)h + to_alloc;
	u32 next_boundry = (new_start_point + 0x1000) & 0xfffff000;
	u32 size_needed = 4096 + next_boundry - new_start_point + sizeof(struct HeapFooter);
	u32 remaining_size = h->size - to_alloc;
	return h->size >= size_needed + to_alloc;
	return remaining_size >= size_needed;
}

#include "io.h"
void* heap_malloc(u32 size, bool align) {
	// Calculate how big of a hole we need, including the header
	// and footer
	u32 full_size = size + sizeof(struct HeapHeader) + sizeof(struct HeapFooter);

	// Loop over all holes
	struct HeapHeader* h;	
	for (h = heap_start; true; h = (struct HeapHeader*)((u32)h + h->size)) {
		u32 next_boundry = ((u32)h + sizeof(struct HeapHeader) + 0x1000) & 0xFFFFF000;
		u32 new_full_size = align ? full_size + next_boundry - (u32)h : full_size;

		/* Make sure we always have enough memory for a new
		 * page table */
		if (!align && h->is_last && !could_fit_page_table(h, new_full_size)) {
			goto correct_exit;
		}

		// Check if memory is available
		if (!h->is_hole) {
			goto correct_exit;
		}
		// Check if hole is big enough
		if (h->size < new_full_size) {
			goto correct_exit;
		}
		// Simple case: If our hole is the exact size we need,
		// fill it
		if (h->size == new_full_size) {
			used_bytes += size;
			h->is_hole = false;
			return (void*)((u32)h + sizeof(struct HeapHeader));
		}

		// If it isn't the exact size, we need to split the
		// hole in two. This code checks if we have room to do
		// that
		u32 remaining_size = h->size - new_full_size;
		if (remaining_size < sizeof(struct HeapHeader) + sizeof(struct HeapFooter)) {
			goto correct_exit;
		}

		// Make a new header for our new hole
		struct HeapHeader* next_header = (struct HeapHeader*)((u32)h + new_full_size);
		next_header->size = remaining_size;
		assert(next_header->size != 0, "heap_malloc: Invalid heap block");
		next_header->is_hole = true;
		next_header->is_last = h->is_last;

		// Modify the existing footer to point to that new
		// header
		struct HeapFooter* next_footer = (struct HeapFooter*)((u32)h + h->size - sizeof(struct HeapFooter));
		next_footer->header = next_header;

		// Make ourselves a new footer.
		struct HeapFooter* new_footer = (struct HeapFooter*)((u32)next_header - sizeof(struct HeapFooter));
		new_footer->header = h;

		// Set up the header correctly
		h->is_hole = false;
		h->is_last = false;
		h->size = new_full_size;
		assert(h->size != 0, "heap_malloc: Invalid heap block");
		if (align) {
			struct HeapHeader* new_a = (struct HeapHeader*)(next_boundry - sizeof(struct HeapHeader));
			new_a->is_hole = false;
			new_a->is_last = false;
			new_a->size = full_size;
			assert(new_a->size != 0, "heap_malloc: Invalid heap block");
			used_bytes += size;
			return (void*)((u32)new_a + sizeof(struct HeapHeader));
		}
		used_bytes += size;
		return (void*)((u32)h + sizeof(struct HeapHeader));
	correct_exit:
		if (h->is_last) {
			break;
		}
	}

	/* We're probably in a loop here. Just return NULL */
	if (align) {
		return NULL;
	}
	/* Try to allocate another page */
	u32 addr = ((u32)h + h->size + 0x000) & 0xfffff000;
	bool res = alloc_page(current_directory, addr);
	/* Allocating a new page might add another header, making the
	 * current header no longer the last one. Correct this. */
	while (!h->is_last) {
		h = (struct HeapHeader*)((u32)h + h->size);
	}
	assert(res, "heap_malloc: Out of memory");
	/* Create a new footer in the new page */
	struct HeapFooter* new_footer = (struct HeapFooter*)((u32)h + h->size - sizeof(struct HeapFooter));
	new_footer->header = h;
	h->size += 4096;
	return heap_malloc(size, align);
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
	u32 block_size = h->size - sizeof(struct HeapHeader) - sizeof(struct HeapFooter);
	used_bytes -= block_size;
	h->is_hole = true;
	// Merge with adjacent holes
	merge_left(h);
	merge_right(h);
}

u32 heap_get_used() {
	return used_bytes;
}
