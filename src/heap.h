#ifndef HEAP_H
#define HEAP_H

#include "common.h"

/**
   A header for a memory heap. The memory heap is used for full sized
   memory allocations.
 */
struct HeapHeader {
	u32 size; /* How big the heap is */
	bool is_hole; /* Whether this heap area is free to store a new allocation */
	bool is_last; /* Whether this is the last section of memory */
};

/**
   A footer that points to the header. This is needed to get the
   previous memory region (I refuse to use a consistent and correct
   term for this).
 */
struct HeapFooter {
	struct HeapHeader* header;
};

/**
   Initialize our heap data.
 */
void heap_init(void*,u32);

/**
   Allocate memory on the heap.
 */
void* heap_malloc(u32, bool);

/**
   Free memory from the heap.
 */
void heap_free(void*);

#endif
