#include "mutex.h"
#include "task.h"

void lock(mutex_t* mtx) {
	/* Deprioritize a task while waiting on a lock. Disable
	 * interrupts for this part of the code */
	asm volatile("cli");
	current_task->priority = 15;
	asm volatile("sti");
	
	u32 res = 1;
	while (res == 1) {
		asm volatile("xchgl %0, %1" : "+m"(*mtx), "+r"(res));
	}
	/* Reprioritize the task when we are done */
	asm volatile("cli");
	current_task->priority = 0;
	asm volatile("sti");
	return;
}

bool try_lock(mutex_t* mtx) {
	u32 res = 1;
	asm volatile("xchg %0, %1" : "+m"(*mtx), "+r"(res));
	return res == 0;
}

void unlock(mutex_t* mtx) {
	// If we have the lock, we don't need to worry about race
	// conditions.
	*mtx = 0;
}
