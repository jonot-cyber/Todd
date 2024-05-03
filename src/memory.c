#include "memory.h"

#include "common.h"
#include "heap.h"
#include "idt.h"
#include "isr.h"
#include "io.h"
#include "mutex.h"
#include "stack_trace.h"

extern u32 end;

// Make sure that all memory (de)allocations happen atomically.
static mutex_t malloc_mtx;

/* Size of RAM */
static u32 memory_size;

struct PageDirectory *current_directory;

static u32 alloc_ptr = (u32)&end;
static bool heap_exists = false;

void* kmalloc_internal(u32 size, bool align, u32 *physical) {
	lock(&malloc_mtx);
	if (heap_exists) {
		void *tmp = heap_malloc(size, align);
		if (physical != NULL)
			*physical = (u32)tmp;
		unlock(&malloc_mtx);
		return tmp;
	}
	if (align)
		alloc_ptr = align4k(alloc_ptr);
	if (physical)
		*physical = alloc_ptr;
	u32 ret = alloc_ptr;
	alloc_ptr += size;
	unlock(&malloc_mtx);
	return (void *)ret;
}

void *kmalloc(u32 size) {
	return kmalloc_internal(size, false, NULL);
}

void *kmalloc_z(u32 size) {
	void *ret = kmalloc(size);
	memset(ret, 0, size);
	return ret;
}

void *kmalloc_a(u32 size) {
	return kmalloc_internal(size, true, NULL);
}

void *kmalloc_p(u32 size, u32 *physical) {
	return kmalloc_internal(size, false, physical);
}

void *kmalloc_ap(u32 size, u32* physical) {
	return kmalloc_internal(size, true, physical);
}

void kfree(void *ptr) {
	lock(&malloc_mtx);
	assert(heap_exists, "kfree: Heap not initialized");
	heap_free(ptr);
	unlock(&malloc_mtx);
}

u32 virtual_to_physical(u32 virt) {
	struct PageTable *tables = (struct PageTable *)0xFFC00000;
	u32 ret = *(u32 *)&tables[virt / 0x1000];
	ret &= 0xFFFFF000;
	return ret + virt % 0x1000;
}

bool alloc_page(struct PageDirectory *dir, u32 addr) {
	if (addr > memory_size - 4096)
		return false;
	u32 frame_addr = addr / 0x1000;
	u32 table_i = frame_addr % 1024; /* address in table */
	u32 dir_i = frame_addr / 1024; /* address in directory */
	struct PageTable *table = (struct PageTable *)(((u32)dir->entries[dir_i]) & 0xfffff000);
	if (!table) {
		/* Manually unlock mutex to prevent deadlock. This is pretty
		 * spooky, but should be fine, because only functions that
		 * have the mutex lock should be calling this. */
		bool is_locked = malloc_mtx;
		if (malloc_mtx)
			unlock(&malloc_mtx);
		table = kmalloc_a(sizeof(struct PageTable));
		/* Restore malloc state */
		if (is_locked)
			lock(&malloc_mtx);
		assert(table != NULL, "alloc_page: Failed to allocate memory for page table");
		memset(table, 0, sizeof(struct PageTable));
		u32 table_u = (u32)table;
		table_u |= PAGE_TABLE_PRESENT | PAGE_TABLE_WRITE;
		dir->entries[dir_i] = (struct PageTable *)table_u;
	}
	table->entries[table_i] = addr | PAGE_FRAME_PRESENT | PAGE_FRAME_WRITE | PAGE_FRAME_USER;
	return true;
}

void memory_init(void *start, u32 bytes) {
	memory_size = bytes;
	const u32 heap_size = 128 * 1024;
	
	if (alloc_ptr < (u32)start)
		alloc_ptr = (u32)start;
	current_directory = kmalloc_a(sizeof(struct PageDirectory));
	memset(current_directory, 0, sizeof(struct PageDirectory));

	u32 it = 0;
	while (it < alloc_ptr + heap_size) {
		alloc_page(current_directory, it);
		it += 0x1000;
	}
	register_interrupt_handler(14, page_fault);

	asm volatile("mov %0, %%cr3" :: "r"(current_directory));
	u32 cr0;
	asm volatile("mov %%cr0, %0" : "=r"(cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0" :: "r"(cr0));

	heap_init((void *)alloc_ptr, heap_size);

	heap_exists = true;
}

void page_fault(struct Registers regs) {
	u32 address;
	asm volatile("mov %%cr2, %0" : "=r"(address));
	printf("PAGE FAULT: 0x%x\n", address);
	printf("eax: 0x%x\n", regs.eax);
	printf("ebx: 0x%x\n", regs.ebx);
	printf("ecx: 0x%x\n", regs.ecx);
	printf("edx: 0x%x\n", regs.edx);
	printf("esp: 0x%x\n", regs.esp);
	printf("ebp: 0x%x\n", regs.ebp);
	printf("eip: 0x%x\n", regs.eip);
	halt();
}
