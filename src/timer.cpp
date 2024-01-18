#include "timer.h"
#include "isr.h"
#include "io.h"
#include "task.h"

volatile u32 ticks = 0;

static void timerCallback(Registers registers) {
	ticks++;
	Task::switchTask();
}

void Timer::init(u32 frequency) {
	ISR::registerInterruptHandler(ISR::IRQ0, &timerCallback);

	u32 divisor = 1193180 / frequency;
	assert(divisor < 0b10000000000000000, "Timer::init: divisor must fit in a 16 bit integer");
	IO::out(0x43, 0x36);
	u8 l = (u8)(divisor & 0xFF);
	u8 h = (u8)((divisor >> 8) & 0xFF);

	IO::out(0x40, l);
	IO::out(0x40, h);
}
