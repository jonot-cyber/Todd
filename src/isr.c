#include "isr.h"

Handler handlers[256];

void register_interrupt_handler(u8 n, Handler handler) {
	handlers[n] = handler;
}
