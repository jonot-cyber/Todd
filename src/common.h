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
   A module for use in multiboot. As of now unused
 */
struct MultiBootModule {
	u32 mod_start;
	u32 mod_end;
	u32 string;
	u32 reserved;
} __attribute__ ((packed));

/**
   Multiboot header information

   A lot of functions aren't implemented, and this is because I don't
   use them.
*/
struct MultiBoot {
	u32 flags;
	u32 mem_lower;
	u32 mem_upper;
	u32 boot_device;
	u32 cmd_line;
	u32 mods_count;
	u32 mods_addr;
	u32 syms[3];
	u32 mmap_length;
	u32 mmap_addr;
	u32 drives_length;
	u32 drives_addr;
	u32 config_table;
	u32 bootloader_name;
	u32 apm_table;
	u32 vbe_control_info;
	u32 vbe_mode_info;
	u16 vbe_mode;
	u16 vbe_interface_seg;
	u16 vbe_interface_off;
	u16 vbe_interface_len;
	u32 framebuffer_addr;
	u32 none;
	u32 framebuffer_pitch;
	u32 framebuffer_width;
	u32 framebuffer_height;
	u32 framebuffer_bpp;
	u32 framebuffer_type;
	u8 color_info[5];
} __attribute__ ((packed));

/**
   Detects whether the multiboot header has memory information present.
 */
bool multiboot_mem_present(struct MultiBoot*);

/**
   Detects whether the multiboot header has boot device information present.
 */
bool multiboot_boot_device_present(struct MultiBoot*);

/**
   Detects whether the multiboot header has command line information present.
 */
bool multiboot_cmd_line_present(struct MultiBoot*);

/**
   Detects whether the multiboot header has modules present.
 */
bool multiboot_mods_present(struct MultiBoot*);

#endif
