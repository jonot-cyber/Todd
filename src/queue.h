#ifndef QUEUE_H
#define QUEUE_H

#include "common.h"
#include "mutex.h"

/*
  Represents a FIFO circular queue. Can be safely used in multi-threaded environments.
 */
struct Queue {
	u32*	data;		/* The data of the queue */
	u32	size;		/* The number of elements that are
				 * currently in the queue */
	u32	cap;		/* The number of elements that the
				 * queue can hold */
	u32	pop_i;		/* The index to pop the next element
				 * from */
	u32	push_i;		/* The index to push the next element
				 * to */
	mutex_t m;		/* A mutex to esure that things work
				 * multi-threaded */
};

/* Initialize a queue in memory. */
void queue_init(struct Queue*, u32);

/* Free a queue */
void queue_free(struct Queue*);

/* Push a value to a queue. Returns false if the queue is full */
bool queue_push(struct Queue*, u32);

/* Pop a value from a queue. Return a null pointer if the queue is empty */
u32* queue_pop(struct Queue*);

#endif
