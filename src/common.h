#ifndef COMMON_H
#define COMMON_H

// Consistency types

// Unsigned 32 bit integer
typedef unsigned int u32;

// Signed 32 bit integer
typedef int i32;

// Unsigned 16 bit integer
typedef unsigned short u16;

// Signed 16 bit integer
typedef short i16;

// Unsigned 8 bit integer
typedef unsigned char u8;

// Signed 8 bit integer
typedef char i8;

// A boolean
typedef u8 bool;

// Make some constants. These are usually in the standard library, but
// there's no standard library where we're going
#define true 1
#define false 0
#define NULL 0

// The initial stack pointer. Used for multitasking I think
extern u32 initial_esp;

/**
   Clears a section of memory to a value
 */
void memset(void*, u8, u32);

/**
   Copies memory from one place to another
 */
void memcpy(const void*, void*, u32);

/**
   Triggers an infinite loop. Used in case of errors.
 */
void halt();

/**
   Aligns a value to a 4k page boundry. I put this here because it's
   used a bit
 */
u32 align4k(u32);

/**
   Assert that a condition is true. Doesn't do anything in release
   mode, but in debug will stop execution and display a message
 */
void assert(bool, const i8*);

/**
   Sleeps for a number of milliseconds
 */
void usleep(u32);

/**
   A module for use in multiboot. As of now unused
 */
struct MultiBootModule {
	u32 mod_start;
	u32 mod_end;
	u32 string;
	u32 reserved;
} __attribute__ ((packed));

#endif
