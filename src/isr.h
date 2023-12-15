#pragma once

#include "idt.h"

namespace ISR {
    enum IRQS: u8 {
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

    typedef void (*Handler)(Registers);

    void registerInterruptHandler(u8 n, Handler handler);

    bool handlerPresent(u8 n);

    Handler getHandler(u8 n);
}