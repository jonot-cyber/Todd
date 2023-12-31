#include "ordered_array.h"

#include "memory.h"

u32 OrderedArray::shadowMalloc(u32 size) {
	return Memory::kmalloc(size);
}

void OrderedArray::shadowFree(void* ptr) {
	return Memory::kfree(ptr);
}
