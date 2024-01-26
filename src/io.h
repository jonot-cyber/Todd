#ifndef IO_H
#define IO_H

#include "common.h"

// Output a byte to an IO port
void io_out(u16, u8);

// Input a byte from an IO port
u8 io_in(u16);

/**
   Input a word from an IO port

   I can't remember if I ever use this.
 */
u16 io_in16(u16);

#endif
