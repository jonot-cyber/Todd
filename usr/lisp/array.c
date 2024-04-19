#include "string.h"
#include "system.h"

#include "array.h"

void array_init(struct Array *a, unsigned elem_size) {
	a->size = 0;
	a->capacity = 1024;
	a->elem_size = elem_size;
	a->data = malloc(elem_size * a->capacity);
}

void array_free(struct Array *a) {
	free(a->data);
}

void array_grow(struct Array *a) {
	if (a->size != a->capacity)
		return;

	void *new_data = malloc(a->elem_size * a->capacity * 2);
	memcpy(a->data, new_data, a->elem_size * a->size);
	free(a->data);
	a->data = new_data;
	a->capacity *= 2;
}

void array_shrink(struct Array *a) {
	if (a->size <= 4 || a->size >= a->capacity / 2)
		return;
	void *new_data = malloc(a->elem_size  * a->capacity / 2);
	memcpy(a->data, new_data, a->elem_size * a->size);
	free(a->data);
	a->data = new_data;
	a->capacity /= 2;
}

void array_push(struct Array *a, void* elem) {
	void *insert_ptr = (void *)((unsigned)a->data + a->size * a->elem_size);
	memcpy(elem, insert_ptr, a->elem_size);
	a->size++;
	array_grow(a);
}

void array_pop(struct Array *a, void *dest) {
	void* pop_ptr = (void *)((unsigned)a->data + (a->size - 1) * a->elem_size);
	memcpy(pop_ptr, dest, a->elem_size);
	a->size--;
	array_shrink(a);
}

void array_remove(struct Array *a, unsigned i) {
	unsigned *arr = (unsigned *)a->data;
	for (unsigned j = i; j < a->size - 1; j++)
		arr[j] = arr[j + 1];
	a->size--;
	// array_shrink(a);
}

void array_insert(struct Array *a, void *elem, unsigned i) {
	array_grow(a);
	memcpy((void *)((unsigned)a->data + i * a->elem_size), (void *)((unsigned)a->data + i * a->elem_size + 1), a->elem_size * (a->size - i));
	memcpy(elem, (void *)((unsigned)a->data + i * a->elem_size), a->elem_size);
	a->size++;
}
