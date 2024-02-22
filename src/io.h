#ifndef IO_H
#define IO_H

#include "common.h"

void io_init(bool enable_monitor, bool enable_serial);
void write_char(i8);
void write_string(const i8*);
void printf(const i8* str, ...);

#endif
