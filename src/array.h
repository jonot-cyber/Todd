#ifndef ARRAY_H
#define ARRAY_H

#include "common.h"

struct Array {
	void* data;
	u32 size;
	u32 elem_size;
	u32 capacity;
};

void array_init(struct Array*, u32 elem_size);

void array_free(struct Array*);

void array_push(struct Array*, void* elem);

void array_pop(struct Array*, void* dest);

void array_remove(struct Array*, u32 i);

void array_insert(struct Array*, void* elem, u32 i);

#endif
