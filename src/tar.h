#ifndef TAR_H
#define TAR_H

#include "common.h"

struct TarHeader {
	u8 file_name[100];
	u8 file_mode[8];
	u8 user_id[8];
	u8 group_id[8];
	u8 file_size[12];
	u8 modification_time[12];
	u8 checksum[8];
	u8 type_flag;
	u8 link_name[100];
	u8 ustar_indiator[6];
	u8 ustar_version[2];
	u8 user_name[32];
	u8 group_name[32];
	u8 device_major_number[8];
	u8 device_minor_number[8];
	u8 filename_prefix[155];
	/* Used to pad the structure to 512 bytes */
	u8 buffer_data[12];
	u8 data_ptr;
} __attribute__((packed));

/* Represents the needed information of a file. Linked list */
struct FSNode {
	i8* name;
	u32 file_size;
	u8* data;
	struct FSNode* next;
};

void load_ustar(struct TarHeader* header);

struct FSNode* tar_find_file(struct FSNode* root, i8 const* const name);

extern struct FSNode* fs_root;

#endif
