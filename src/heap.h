#ifndef HEAP_H
#define HEAP_H

#include "common.h"

struct HeapHeader {
	u32 size;
	bool is_hole;
	bool is_last;
};

struct HeapFooter {
	struct HeapHeader* header;
};

void heap_init(void*,u32);
void* heap_malloc(u32);
void heap_free(void*);

#endif
