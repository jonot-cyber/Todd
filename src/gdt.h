#ifndef GDT_H
#define GDT_H

#include "common.h"

/**
   Flush the GDT information to the CPU
 */
extern void GDTFlush(u32);

/**
   Setup the GDT headers
 */
void gdt_init();

/**
   Stores a GDT entry. This is the kind of structure that makes you
   hate Intel.
 */
struct GDTEntry {
	u16 limit_low;
	u16 base_low;
	u8 base_middle;
	u8 access;
	u8 granularity;
	u8 base_high;
} __attribute__((packed));

/**
   Pointer to the GDT structures.
 */
struct GDTPointer {
	u16 limit;
	u32 base;
} __attribute__((packed));

#endif 
