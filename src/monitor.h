#ifndef MONITOR_H
#define MONITOR_H

#include "common.h"
#include "vga.h"

extern enum VGAColor background_color;
extern enum VGAColor foreground_color;

void monitor_init();
void write_char(i8);
void write_string(const i8*);
void reset_color();
void write_hex(u8);
void write_hex16(u16);
void write_hex32(u32);
void write_dec(u32);
void write_bin(u32);
void printf(const i8* str, ...);

#endif
