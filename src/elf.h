#ifndef ELF_H
#define ELF_H

#include "common.h"

struct ElfHeader {
	u8 magic_number[4]; /* Should be 0x74, then "ELF" in ascii */
	u8 word_size; /* 1 = 32 bit, 2 = 64 bit. Should be 1 */
	u8 endian; /* 1 = LE, 2 = BE. x86 is LE, so should be 1 */
	u8 header_version;
	u8 os_abi;
	u8 padding[8];
	u8 type[2]; /* type, 1 = relocatable, 2 = executable, 3 = shared, 4 = core */
	u8 instruction_set[2]; /* should be 0x03 */
	u32 elf_version; /* should just be 1 */
	u32 program_entry_offset; /* offset where we start executing? */
	u32 program_header_table_offset;
	u32 section_header_table_offset;
	u32 flags; /* TODO: fill in */
	u16 header_size;
	u16 program_header_table_entry_size;
	u16 program_header_table_entry_count;
	u16 section_header_table_entry_size;
	u16 section_header_table_entry_count;
	u16 section_name_index;
} __attribute__((packed));

enum ElfProgramHeaderFlags {
	PF_X = 0x1,
	PF_W = 0x2,
	PF_R = 0x4,
};

struct ElfProgramHeader {
	u32 p_type; /* Segment type */
	u32 p_offset; /* Offset of the segment in the file image. */
	u32 p_vaddr; /* Virtual address of the segment in memory. */
	u32 p_paddr; /* Physical address, might not matter */
	u32 p_filesz; /* Size in bytes of segment in file */
	u32 p_memsz; /* Size in bytes of segment in memory */
	u32 p_flags; /* Whether the segment is executable, writeable, and readable */
	u32 p_align; /* The alignment of the block */
} __attribute__((packed));

enum ElfSectionHeaderType {
	SHT_NULL = 0x0, /* Unused */
	SHT_PROGBITS = 0x1, /* Program Data */
	SHT_SYMTAB = 0x2, /* Symbol Table */
	SHT_STRTAB = 0x3, /* String table */
	SHT_RELA = 0x4, /* Relocation entries */
	SHT_HASH = 0x5, /* Symbol hash table */
	SHT_DYNAMIC = 0x6, /* Dynamic linking information */
	SHT_NOTE = 0x7, /* Notes */
	SHT_NOBITS = 0x8, /* Program space, no data */
	SHT_REL = 0x9, /* Relocation entries, no addends */
};

enum ElfSectionHeaderFlags {
	SHF_WRITE = 0x1,
	SHF_ALLOC = 0x2,
	SHF_EXECINSTR = 0x4,
	SHF_MERGE = 0x10,
	SHF_STRINGS = 0x20,
	SHF_INFO_LINK = 0x40,
	SHF_LINK_ORDER = 0x80,
	SHF_OS_NONCONFORMING = 0x100,
	SHF_GROUP = 0x200,
	SHF_TLS = 0x400,
};

struct ElfSectionHeader {
	u32 sh_name; /* Offset to a string in .shstrtab that represents the name */
	u32 sh_type; /* The type of the header */
	u32 sh_flags; /* Attributes of the section */
	u32 sh_addr; /* Virtual address of th section in memory */
	u32 sh_offset; /* The offset of th section in the image */
	u32 sh_size; /* The size of the section */
	u32 sh_link; /* Index of associated section */
	u32 sh_info; /* Extra information about the section */
	u32 sh_addralign; /* Alignment of th section */
	u32 sh_entsize; /* Huh */
} __attribute__((packed));

void elf_load(void* data, u32 size);

#endif
