#include "tar.h"

#include "string.h"
#include "io.h"

/* Read octal number from a buffer */
u32 read_octal(u8* data, u32 len) {
	u32 ret = 0;
	for (u32 i = 0; i < len; i++) {
		ret *= 8;
		ret += data[i] - '0';
	}
	return ret;
}

void ustar_info(struct TarHeader *header) {
	printf("File Name: %s\n", header->file_name);
	printf("File Mode: %s\n", header->file_mode);
	printf("User ID: %d\n", read_octal(header->user_id, 8));
	printf("Group ID: %d\n", read_octal(header->group_id, 8));
	printf("File Size: %d\n", read_octal(header->file_size, 12));
	printf("Modification Time: %d\n", read_octal(header->modification_time, 12));
	printf("Checksum: %d\n", read_octal(header->checksum, 8));
	printf("Type Flag: %c\n", header->type_flag);
	printf("Link name: %s\n", header->link_name);
	printf("UStar indicator: %s\n", header->ustar_indiator);
	assert(strcmp_span((i8*)header->ustar_indiator, "ustar", 5) == 0, "ustar_info: Archive isn't ustar. Is it 1979?");
	printf("UStar version: %s\n", header->ustar_version);
	printf("User Name: %s\n", header->user_name);
	printf("Group Name: %s\n", header->group_name);
	printf("Device Major Number: %s\n", header->device_major_number);
	printf("Device Minor Number: %s\n", header->device_minor_number);
	printf("Filename Prefix: %s\n", header->filename_prefix);
}
