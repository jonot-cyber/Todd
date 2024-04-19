#ifndef ARRAY_H
#define ARRAY_H

struct Array {
	void *data;
	unsigned size;
	unsigned elem_size;
	unsigned capacity;
};

void array_init(struct Array *, unsigned elem_size);

void array_free(struct Array *);

void array_push(struct Array *, void *elem);

void array_pop(struct Array *, void *dest);

void array_remove(struct Array *, unsigned i);

void array_insert(struct Array *, void *elem, unsigned i);

#endif
