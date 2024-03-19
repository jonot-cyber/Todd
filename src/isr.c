#include "isr.h"

#include "io.h"

static Handler handlers[256];
void register_interrupt_handler(u8 n, Handler handler) {
	// This shouldn't be a function
	handlers[n] = handler;
}

Handler get_handler(u8 n) {
	return handlers[n];
}
