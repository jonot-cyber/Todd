#include "timer.h"

#include "isr.h"
#include "io_port.h"
#include "monitor.h"
#include "task.h"

volatile u32 ticks = 0;

static void timer_callback(struct Registers registers) {
	ticks++;
	if (ticks % 1 == 0) {		
		switch_task();
	}
	UNUSED(registers);
}

void timer_init(u32 frequency) {
	register_interrupt_handler(IRQ0, &timer_callback);

	u32 divisor = 1193180 / frequency;
	assert(divisor < 0b10000000000000000, "timer_init: divisor must fit in a 16 bit integer");
	io_out(0x43, 0x36);
	u8 l = (u8)(divisor & 0xFF);
	u8 h = (u8)((divisor >> 8) & 0xFF);

	io_out(0x40, l);
	io_out(0x40, h);
	//asm volatile("sti");
}
