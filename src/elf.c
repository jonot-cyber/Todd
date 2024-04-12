#include "elf.h"

#include "io.h"
#include "string.h"
#include "task.h"

void remap_global_offset_table(u32 entry_point, u32 *addr, u32 size) {
	addr--;
	for (u32 i = 0; i < size / 4; i++) {
		addr[i] += entry_point;
	}
}

void elf_load(void *data) {
	struct ElfHeader *header = data;
	/* Verification */
	assert(header->magic_number[0] = 0x74, "elf_load: Bad magic!");
	assert(header->magic_number[0] = 'E', "elf_load: Bad magic!");
	assert(header->magic_number[0] = 'L', "elf_load: Bad magic!");
	assert(header->magic_number[0] = 'F', "elf_load: Bad magic!");
	assert(header->word_size == 1, "elf_load: Can't load a 64 bit executable");
	assert(header->endian == 1, "elf_load: Big Endian executable");
	assert(header->type[0] == 2, "elf_load: Not an executable");
	assert(header->instruction_set[0] == 0x03, "elf_load: Not x86");
	assert(header->elf_version == 1, "elf_load: Weird elf version");

	u32 addr = (u32)data + header->program_header_table_offset;
	struct ElfProgramHeader *p_headers = (struct ElfProgramHeader*)addr;
	u32 entry_point = header->program_entry_offset;
	void *exec_offset = NULL;
	/* The entry point is based on the virtual memory, which we
	 * aren't setting up. We need to figure out which segment it
	 * is in, and offset it. */
	for (u32 i = 0; i < header->program_header_table_entry_count; i++) {
		struct ElfProgramHeader p_header = p_headers[i];
		u32 begin = p_header.p_vaddr;
		u32 end = begin + p_header.p_memsz;
		if (entry_point >= begin && entry_point < end) {
			entry_point += p_header.p_offset - begin;
			exec_offset = p_header.p_offset + data;
			break;
		}
	}

	/* Remap the global offset table */
	struct ElfSectionHeader *s_headers = (struct ElfSectionHeader *)(data + header->section_header_table_offset);
	/* The index of the names section */
	struct ElfSectionHeader sname_header = s_headers[header->section_name_index];
	if (!header->has_been_run)
		for (u32 i = 0; i < header->section_header_table_entry_count; i++) {
			struct ElfSectionHeader s_header = s_headers[i];
			/* Get the name from the symbol table */
			const i8 *name= sname_header.sh_offset + s_header.sh_name + data;
			/* If we found the global offset table, offset all the addresses */
			if (strcmp(name, ".got.plt") == 0) {
				remap_global_offset_table((u32)exec_offset,
							  (u32*)(s_header.sh_offset + (i8*)data),
							s_header.sh_size);
			}
		}
	assert(entry_point != header->program_entry_offset, "elf_load: Failed to find entry point");
	/* Get the offset into the file data */
	entry_point += (u32)data;
	/* Set this variable so we don't offset the global offset
	 * table more than once */
	header->has_been_run = 1;
	asm volatile("call *%0" :: "r"(entry_point));
}
