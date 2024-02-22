#ifndef SERIAL_H
#define SERIAL_H

#include "common.h"

/* Initialize the COM1 port */
void serial_init();

/* Write a character to the serial port */
void serial_write_char(i8 c);

#endif
