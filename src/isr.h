#ifndef ISR_H
#define ISR_H

#include "idt.h"
#include "common.h"

typedef void (*Handler)(struct Registers);

extern Handler handlers[256];

enum IRQS {
	IRQ0 = 32,
	IRQ1 = 33,
	IRQ2 = 34,
	IRQ3 = 35,
	IRQ4 = 36,
	IRQ5 = 37,
	IRQ6 = 38,
	IRQ7 = 39,
	IRQ8 = 40,
	IRQ9 = 41,
	IRQ10 = 42,
	IRQ11 = 43,
	IRQ12 = 44,
	IRQ13 = 45,
	IRQ14 = 46,
	IRQ15 = 47,
};

void register_interrupt_handler(u8 n, Handler handler);

#endif
