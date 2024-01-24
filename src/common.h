#ifndef COMMON_H
#define COMMON_H

// Some types to be used for consistency
typedef unsigned int u32;
typedef int i32;
typedef unsigned short u16;
typedef short i16;
typedef unsigned char u8;
typedef char i8;
#ifndef __cplusplus
typedef u8 bool;
#endif

#define true 1
#define false 0
#define NULL 0

extern u32 initial_esp;

/**
   Clears a section of memory to a value

   @param start is the memory address to start filling at
   @param value is what value to fill the range with
   @param size is how many bytes to fill with value
 */
void memset(void* start, u8 value, u32 size);

/**
   Copies memory from one place to another

   @param src is where to copy from
   @param dst is where to copy to
   @param size is how many bytes to copy
 */
void memcpy(const void* src, void* dst, u32 size);

/**
   Triggers an infinite loop. Used in case of errors.
 */
void halt();

/**
   Aligns a value to a 4k page boundry. I put this here because it's
   used a bit

   @param in the addr to take in
 */
u32 align4k(u32 in);

/**
   Assert that a condition is true. Doesn't do anything in release
   mode, but in debug will stop execution and display a message
 */
void assert(bool condition, const i8* message);

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

bool multiboot_mem_present(struct MultiBoot*);
bool multiboot_boot_device_present(struct MultiBoot*);
bool multiboot_cmd_line_present(struct MultiBoot*);
bool multiboot_mods_present(struct MultiBoot*);

#endif
