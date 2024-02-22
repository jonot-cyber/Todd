#ifndef MONITOR_H
#define MONITOR_H

#include "common.h"
#include "vga.h"

extern enum VGAColor background_color;
extern enum VGAColor foreground_color;

void monitor_init();
void monitor_write_char(i8);
void reset_color();

#endif
