#include "isr.h"

Handler handlers[256];

void register_interrupt_handler(u8 n, Handler handler) {
	// This shouldn't be a function
	handlers[n] = handler;
}
