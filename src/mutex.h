#ifndef MUTEX_H
#define MUTEX_H

#include "common.h"

typedef u32 mutex_t;

void lock(mutex_t*);
bool try_lock(mutex_t*);
void unlock(mutex_t*);

#endif
