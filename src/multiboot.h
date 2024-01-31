#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include "common.h"

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
