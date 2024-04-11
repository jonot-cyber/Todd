#include "elf.h"

#include "io.h"

void elf_load(void* data) {
	struct ElfHeader* header = data;
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
	struct ElfProgramHeader* p_headers = (struct ElfProgramHeader*)addr;
	u32 entry_point = header->program_entry_offset;
	for (u32 i = 0; i < header->program_header_table_entry_count; i++) {
		struct ElfProgramHeader p_header = p_headers[i];
		u32 begin = p_header.p_vaddr;
		u32 end = begin + p_header.p_memsz;
		if (entry_point >= begin && entry_point < end) {
			entry_point -= begin;
			entry_point += p_header.p_offset;
			break;
		}
	}
	assert(entry_point != header->program_entry_offset, "elf_load: Failed to find entry point");
	/* Get the offset into the file data */
	entry_point += (u32)data;
	asm volatile("call *%0" :: "r"(entry_point));
}