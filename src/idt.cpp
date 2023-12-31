#include "idt.h"
#include "isr.h"
#include "monitor.h"
#include "io.h"

// IO ports for the Programmable Interrupt Controller
const u16 PRIMARY_CMD = 0x20;
const u16 PRIMARY_DATA = 0x21;
const u16 SECONDARY_CMD = 0xA0;
const u16 SECONDARY_DATA = 0xA1;

// IDT entries
static IDT::Entry entries[256];
static IDT::Pointer ptr;

/**
   Remap the PIC's interrupt requests don't interfere with CPU interrupts
*/
void remapIRQTable() {
	IO::out(PRIMARY_CMD, 0x11);
	IO::out(SECONDARY_CMD, 0x11);
	IO::out(PRIMARY_DATA, 0x20);
	IO::out(SECONDARY_DATA, 0x28);
	IO::out(PRIMARY_DATA, 0x04);
	IO::out(SECONDARY_DATA, 0x02);
	IO::out(PRIMARY_DATA, 0x01);
	IO::out(SECONDARY_DATA, 0x01);
	IO::out(PRIMARY_DATA, 0x00);
	IO::out(SECONDARY_DATA, 0x00);
}

void IDT::init() {
	ptr.limit = sizeof(Entry) * 256 - 1;
	ptr.base = (u32)&entries;

	memset((u8*)&entries, 0, sizeof(Entry) * 256);

	Flags flags = (Flags)0x8E;
	entries[0].set((u32)isr0, 0x08, flags);
	entries[1].set((u32)isr1, 0x08, flags);
	entries[2].set((u32)isr2, 0x08, flags);
	entries[3].set((u32)isr3, 0x08, flags);
	entries[4].set((u32)isr4, 0x08, flags);
	entries[5].set((u32)isr5, 0x08, flags);
	entries[6].set((u32)isr6, 0x08, flags);
	entries[7].set((u32)isr7, 0x08, flags);
	entries[8].set((u32)isr8, 0x08, flags);
	entries[9].set((u32)isr9, 0x08, flags);
	entries[10].set((u32)isr10, 0x08, flags);
	entries[11].set((u32)isr11, 0x08, flags);
	entries[12].set((u32)isr12, 0x08, flags);
	entries[13].set((u32)isr13, 0x08, flags);
	entries[14].set((u32)isr14, 0x08, flags);
	entries[15].set((u32)isr15, 0x08, flags);
	entries[16].set((u32)isr16, 0x08, flags);
	entries[17].set((u32)isr17, 0x08, flags);
	entries[18].set((u32)isr18, 0x08, flags);
	entries[19].set((u32)isr19, 0x08, flags);
	entries[20].set((u32)isr20, 0x08, flags);
	entries[21].set((u32)isr21, 0x08, flags);
	entries[22].set((u32)isr22, 0x08, flags);
	entries[23].set((u32)isr23, 0x08, flags);
	entries[24].set((u32)isr24, 0x08, flags);
	entries[25].set((u32)isr25, 0x08, flags);
	entries[26].set((u32)isr26, 0x08, flags);
	entries[27].set((u32)isr27, 0x08, flags);
	entries[28].set((u32)isr28, 0x08, flags);
	entries[29].set((u32)isr29, 0x08, flags);
	entries[30].set((u32)isr30, 0x08, flags);
	entries[31].set((u32)isr31, 0x08, flags);

	remapIRQTable();
	entries[32].set((u32)irq0, 0x08, flags);
	entries[33].set((u32)irq1, 0x08, flags);
	entries[34].set((u32)irq2, 0x08, flags);
	entries[35].set((u32)irq3, 0x08, flags);
	entries[36].set((u32)irq4, 0x08, flags);
	entries[37].set((u32)irq5, 0x08, flags);
	entries[38].set((u32)irq6, 0x08, flags);
	entries[39].set((u32)irq7, 0x08, flags);
	entries[40].set((u32)irq8, 0x08, flags);
	entries[41].set((u32)irq9, 0x08, flags);
	entries[42].set((u32)irq10, 0x08, flags);
	entries[43].set((u32)irq11, 0x08, flags);
	entries[44].set((u32)irq12, 0x08, flags);
	entries[45].set((u32)irq13, 0x08, flags);
	entries[46].set((u32)irq14, 0x08, flags);
	entries[47].set((u32)irq15, 0x08, flags);

	flush(&ptr);
	asm volatile("sti"); // Enable interrupts
}

void IDT::Entry::set(u32 base, u16 sel, IDT::Flags f) {
	baseLow = base & 0xFFFF;
	baseHigh = (base >> 16) & 0xFFFF;

	selector = sel;
	always0 = 0;
	flags = f;
}

void IDT::flush(IDT::Pointer* v) {
	IDTFlush((u32)v);
}

/**
   Handle an interrupt
*/
void isrHandler(Registers registers) {
	// If there's a handler, call it instead
	if (ISR::handlerPresent(registers.intNo)) {
		ISR::Handler handler = ISR::getHandler(registers.intNo);
		handler(registers);
		return;
	}
	Monitor::writeChar('\n');
	for (u8 i = 0; i < 80; i++) {
		Monitor::writeChar('=');
	}
	Monitor::printf("\nReceived Unhandled Interrupt: %d\n", registers.intNo);
	for (u8 i = 0; i < 80; i++) {
		Monitor::writeChar('=');
	}
	Monitor::writeChar('\n');
}

/**
   Handle an IRQ interrupt
 */
void irqHandler(Registers registers) {
	// Reset PIC
	if (registers.intNo >= 40) {
		// Send reset signal to secondary
		IO::out(0xA0, 0x20);
	}
	// Send reset signal to primary
	IO::out(0x20, 0x20);

	// If there's a handler, call it.
	if (ISR::handlerPresent(registers.intNo)) {
		ISR::Handler handler = ISR::getHandler(registers.intNo);
		handler(registers);
	}
}
