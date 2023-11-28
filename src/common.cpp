#include "common.h"

void memset(u8* start, u8 v, u32 sz) {
    for (u32 i = 0; i < sz; i++) {
        *(start+i) = v;
    }
}

void halt() {
    while (true); // jail
    asm volatile ("hlt");
}

bool MultiBoot::memPresent() {
    return flags & (1 << 0);
}

bool MultiBoot::bootDevicePresent() {
    return flags & (1 << 1);
}

bool MultiBoot::cmdLinePresent() {
    return flags & (1 << 2);
}

bool MultiBoot::modsPresent() {
    return flags & (1 << 3);
}