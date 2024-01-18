#include "gdt.h"

// The GDT entries
static GDT::Entry entries[5];

// The GDT pointer
static GDT::Pointer ptr;

void GDT::Entry::set(u32 base, u32 limit, GDT::AccessByte a, GDT::GranularityByte gran) {
	// Puts the data from the base into the weird structure
	baseLow = (base & 0xFFFF);
	baseMiddle = (base >> 16) & 0xFF;
	baseHigh = (base >> 24) & 0xFF;

	// Put in the rest of the data
	limitLow = (limit & 0xFFFF);
	granularity = (limit >> 16) & 0x0F;
	granularity |= gran & 0xF0;
	access = a;
}

void GDT::init() {
	// Set up the pointer data
	ptr.limit = (sizeof(GDT::Entry) * 5) - 1;
	ptr.base = (u32)&entries;

	GranularityByte granularityByte = (GranularityByte)(Granularity1k | Operand32 | 0xF);
	entries[0].set(0, 0, static_cast<AccessByte>(0), static_cast<GranularityByte>(0));
	entries[1].set(0, 0xFFFFFFFF, static_cast<AccessByte>(GDT::AccessByte::Present | GDT::AccessByte::DescriptorType | 10), granularityByte);
	entries[2].set(0, 0xFFFFFFFF, static_cast<AccessByte>(GDT::AccessByte::Present | GDT::AccessByte::DescriptorType | 2), granularityByte);
	entries[3].set(0, 0xFFFFFFFF, static_cast<AccessByte>(GDT::AccessByte::Present | GDT::AccessByte::Ring3 | GDT::AccessByte::DescriptorType | 10), granularityByte);
	entries[4].set(0, 0xFFFFFFFF, static_cast<AccessByte>(GDT::AccessByte::Present | GDT::AccessByte::Ring3 | GDT::AccessByte::DescriptorType | 2), granularityByte);

	GDT::flush(&ptr);
}

void GDT::flush(GDT::Pointer* ptr) {
	assert(ptr != nullptr, "GDT::Flush: GDT Pointer is null");
	GDTFlush((u32)ptr);
}
