/* Support for FAT32 file systems */

#ifndef FAT_H
#define FAT_H

#include "common.h"

/* This structure is at the beginning of a FAT32 partition */
struct FatBootSector {
	u8 boot_jmp[3]; /* Here for legacy purposes. */
	u8 oem_name[8]; /* What created the partition. For us, this will probably be mkfs.fat */
	u16 bytes_per_sector;
	u8 sectors_per_cluster;
	u16 reserved_sector_count;
	u8 table_count;
	u16 root_entry_count;
	u16 total_sectors;
	u8 media_type;
	u16 table_size;
	u16 sectors_per_track;
	u16 head_size_count;
	u32 hidden_sector_count;
	u32 total_sectors_32;

	/* Extended section */
	u32 table_size_32;
	u16 extended_flags;
	u16 fat_version;
	u32 root_cluster;
	u16 fat_info;
	u16 backup_bs_sector;
	u8 reserved_0[12];
	u8 drive_number;
	u8 reserved_1;
	u8 boot_signature;
	u32 volume_id;
	u8 volume_label[11];
	u8 fat_type_label[8];
} __attribute__((packed));

void fat_info(struct FatBootSector* bs);

#endif
