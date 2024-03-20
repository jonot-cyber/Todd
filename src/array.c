#include "array.h"

#include "memory.h"

void array_init(struct Array* a, u32 elem_size) {
	a->size = 0;
	a->capacity = 1024;
	a->elem_size = elem_size;
	a->data = kmalloc(elem_size * a->capacity);
}

void array_free(struct Array* a) {
	kfree(a->data);
}

void array_grow(struct Array* a) {
	if (a->size == a->capacity) {
		void* new_data = kmalloc(a->elem_size * a->capacity * 2);
		memcpy(a->data, new_data, a->elem_size * a->size);
		kfree(a->data);
		a->data = new_data;
		a->capacity *= 2;
	}
}

void array_shrink(struct Array* a) {
	if (a->size > 4 && a->size < a->capacity / 2) {
		void* new_data = kmalloc(a->elem_size  * a->capacity / 2);
		memcpy(a->data, new_data, a->elem_size * a->size);
		kfree(a->data);
		a->data = new_data;
		a->capacity /= 2;
	}
}

void array_push(struct Array* a, void* elem) {
	void* insert_ptr = (void*)((u32)a->data + a->size * a->elem_size);
	memcpy(elem, insert_ptr, a->elem_size);
	a->size++;
	array_grow(a);
}

void array_pop(struct Array* a, void* dest) {
	void* pop_ptr = (void*)((u32)a->data + (a->size - 1) * a->elem_size);
	memcpy(pop_ptr, dest, a->elem_size);
	a->size--;
	array_shrink(a);
}

void array_remove(struct Array* a, u32 i) {
	u32* arr = (u32*)a->data;
	for (u32 j = i; j < a->size - 1; j++) {
		arr[j] = arr[j + 1];
	}
	a->size--;
	// array_shrink(a);
}

void array_insert(struct Array* a, void* elem, u32 i) {
	array_grow(a);
	memcpy((void*)((u32)a->data + i * a->elem_size), (void*)((u32)a->data + i * a->elem_size + 1), a->elem_size * (a->size - i));
	memcpy(elem, (void*)((u32)a->data + i * a->elem_size), a->elem_size);
	a->size++;
}
