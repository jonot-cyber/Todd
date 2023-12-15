#include "timer.h"
#include "isr.h"
#include "monitor.h"

static u32 tick = 0;

static void timerCallback(Registers registers) {
    tick++;
    Monitor::putString("Tick: ");
    Monitor::writeDec(tick);
    Monitor::putChar('\n');
}

void Timer::init(u32 frequency) {
    ISR::registerInterruptHandler(ISR::IRQ0, &timerCallback);

    u32 divisor = 1193180 / frequency;
    IO::out(0x43, 0x36);
    u8 l = (u8)(divisor & 0xFF);
    u8 h = (u8)((divisor >> 8) & 0xFF);

    IO::out(0x40, l);
    IO::out(0x40, h);
}
