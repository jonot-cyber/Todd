#include "fat.h"

#include "io.h"

void fat_info(struct FatBootSector* bs) {
	i8 oem_name_buf[9];
	oem_name_buf[8] = '\0';
	memcpy(bs->oem_name, oem_name_buf, 8);
	printf("OEM: %s\n", oem_name_buf);
	printf("bytes/sector: %d\n", bs->bytes_per_sector);
	printf("sectors/cluster: %d\n", bs->sectors_per_cluster);
	printf("reserved sectors: %d\n", bs->reserved_sector_count);
	printf("tables: %d\n", bs->table_count);
	printf("root entries: %d\n", bs->root_entry_count);
	printf("sectors: %d\n", bs->total_sectors_32);
	printf("sectors: %d\n", bs->total_sectors);
}
