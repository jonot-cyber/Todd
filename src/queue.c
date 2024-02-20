#include "queue.h"

#include "memory.h"

void queue_init(struct Queue* q, u32 cap) {
	q->data = kmalloc(sizeof(u32) * cap);
	q->size = 0;
	q->cap = cap;
	q->push_i = 0;
	q->pop_i = 0;
	q->m = 0;
	return;
}

void queue_free(struct Queue* q) {
	lock(&q->m);
	kfree(q->data);
	unlock(&q->m);
}

bool queue_push(struct Queue* q, u32 elem) {
	lock(&q->m);
	if (q->size >= q->cap) {
		unlock(&q->m);
		return false;
	}
	u32 old_i = q->push_i;
	q->push_i = (q->push_i + 1) % ++q->size;
	unlock(&q->m);
	q->data[old_i] = elem;
	return true;
}

u32* queue_pop(struct Queue* q) {
	lock(&q->m);
	if (q->size == 0) {
		return NULL;
	}
	u32 old_i = q->pop_i;
	q->pop_i = (q->pop_i + 1) % q->size--;
	
	unlock(&q->m);
	return &q->data[old_i];
}
