#include "isr.h"

static ISR::Handler isrHandlers[256];

void ISR::registerInterruptHandler(u8 n, ISR::Handler handler) {
    isrHandlers[n] = handler;
}

bool ISR::handlerPresent(u8 n) {
    return isrHandlers[n] != 0;
}

ISR::Handler ISR::getHandler(u8 n) {
    return isrHandlers[n];
}