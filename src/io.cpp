#include "io.h"

IO::IO(u16 port) {
    this->port = port;
}

/* Write u8 to an IO port */
void IO::operator<<(u8 value) const {
    asm volatile("outb %1, %0" : : "dN" (port), "a" (value));
}

/* Write u16 to an IO port */
void IO::operator<<(u16 value) const {
    u8 low = value;
    u8 high = value >> 8;
    *this << high;
    *this << low;
}

/* Read u8 from an IO port */
void IO::operator>>(u8& value) const {
    asm volatile("inb %1, %0" : "=a" (value) : "dN" (port));
}

/* Read u16 from an IO port */
void IO::operator>>(u16& value) const {
    asm volatile("inw %1, %0" : "=a" (value) : "dN" (port));
}