#include "idt.h"
#include "isr.h"
#include "io.h"
#include "io_port.h"

// IO ports for the Programmable Interrupt Controller
const u16 PRIMARY_CMD = 0x20;
const u16 PRIMARY_DATA = 0x21;
const u16 SECONDARY_CMD = 0xA0;
const u16 SECONDARY_DATA = 0xA1;

// IDT entries
static struct IDTEntry entries[256];
static struct IDTPointer ptr;

/**
   Remap the PIC's interrupt requests don't interfere with CPU
   interrupts
*/
void remap_irq_table() {
	// Don't ask me what these numbers mean
	io_out(PRIMARY_CMD, 0x11);
	io_out(SECONDARY_CMD, 0x11);
	io_out(PRIMARY_DATA, 0x20);
	io_out(SECONDARY_DATA, 0x28);
	io_out(PRIMARY_DATA, 0x04);
	io_out(SECONDARY_DATA, 0x02);
	io_out(PRIMARY_DATA, 0x01);
	io_out(SECONDARY_DATA, 0x01);
	io_out(PRIMARY_DATA, 0x00);
	io_out(SECONDARY_DATA, 0x00);
}

void idt_init() {
	ptr.limit = sizeof(struct IDTEntry) * 256 - 1;
	ptr.base = (u32)&entries;

	memset(&entries, 0, sizeof(struct IDTEntry) * 256);

	// Why do I make enums for these things and not use them?
	enum IDTFlags flags = (enum IDTFlags)0x8E;

	// Repetetive code.
	set_idt_entry(&entries[0], (u32)isr0, 0x08, flags);
	set_idt_entry(&entries[1], (u32)isr1, 0x08, flags);
	set_idt_entry(&entries[2], (u32)isr2, 0x08, flags);
	set_idt_entry(&entries[3], (u32)isr3, 0x08, flags);
	set_idt_entry(&entries[4], (u32)isr4, 0x08, flags);
	set_idt_entry(&entries[5], (u32)isr5, 0x08, flags);
	set_idt_entry(&entries[6], (u32)isr6, 0x08, flags);
	set_idt_entry(&entries[7], (u32)isr7, 0x08, flags);
	set_idt_entry(&entries[8], (u32)isr8, 0x08, flags);
	set_idt_entry(&entries[9], (u32)isr9, 0x08, flags);
	set_idt_entry(&entries[10], (u32)isr10, 0x08, flags);
	set_idt_entry(&entries[11], (u32)isr11, 0x08, flags);
	set_idt_entry(&entries[12], (u32)isr12, 0x08, flags);
	set_idt_entry(&entries[13], (u32)isr13, 0x08, flags);
	set_idt_entry(&entries[14], (u32)isr14, 0x08, flags);
	set_idt_entry(&entries[15], (u32)isr15, 0x08, flags);
	set_idt_entry(&entries[16], (u32)isr16, 0x08, flags);
	set_idt_entry(&entries[17], (u32)isr17, 0x08, flags);
	set_idt_entry(&entries[18], (u32)isr18, 0x08, flags);
	set_idt_entry(&entries[19], (u32)isr19, 0x08, flags);
	set_idt_entry(&entries[20], (u32)isr20, 0x08, flags);
	set_idt_entry(&entries[21], (u32)isr21, 0x08, flags);
	set_idt_entry(&entries[22], (u32)isr22, 0x08, flags);
	set_idt_entry(&entries[23], (u32)isr23, 0x08, flags);
	set_idt_entry(&entries[24], (u32)isr24, 0x08, flags);
	set_idt_entry(&entries[25], (u32)isr25, 0x08, flags);
	set_idt_entry(&entries[26], (u32)isr26, 0x08, flags);
	set_idt_entry(&entries[27], (u32)isr27, 0x08, flags);
	set_idt_entry(&entries[28], (u32)isr28, 0x08, flags);
	set_idt_entry(&entries[29], (u32)isr29, 0x08, flags);
	set_idt_entry(&entries[30], (u32)isr30, 0x08, flags);
	set_idt_entry(&entries[31], (u32)isr31, 0x08, flags);

	remap_irq_table();
	set_idt_entry(&entries[32], (u32)irq0, 0x08, flags);
	set_idt_entry(&entries[33], (u32)irq1, 0x08, flags);
	set_idt_entry(&entries[34], (u32)irq2, 0x08, flags);
	set_idt_entry(&entries[35], (u32)irq3, 0x08, flags);
	set_idt_entry(&entries[36], (u32)irq4, 0x08, flags);
	set_idt_entry(&entries[37], (u32)irq5, 0x08, flags);
	set_idt_entry(&entries[38], (u32)irq6, 0x08, flags);
	set_idt_entry(&entries[39], (u32)irq7, 0x08, flags);
	set_idt_entry(&entries[40], (u32)irq8, 0x08, flags);
	set_idt_entry(&entries[41], (u32)irq9, 0x08, flags);
	set_idt_entry(&entries[42], (u32)irq10, 0x08, flags);
	set_idt_entry(&entries[43], (u32)irq11, 0x08, flags);
	set_idt_entry(&entries[44], (u32)irq12, 0x08, flags);
	set_idt_entry(&entries[45], (u32)irq13, 0x08, flags);
	set_idt_entry(&entries[46], (u32)irq14, 0x08, flags);
	set_idt_entry(&entries[47], (u32)irq15, 0x08, flags);
	struct IDTPointer* p = &ptr;
	IDTFlush(*(u32*)&p);
	asm volatile("sti"); // Enable interrupts
}

void set_idt_entry(struct IDTEntry* e, u32 base, u16 sel, enum IDTFlags f) {
	// Intel :)
	e->base_low = base & 0xFFFF;
	e->base_high = (base >> 16) & 0xFFFF;

	e->selector = sel;
	e->always0 = 0;
	e->flags = f;
}

/**
   Handle an interrupt
*/
void isrHandler(struct Registers registers) {
	// If there's a handler, call it instead
	if (handlers[registers.intNo]) {
		Handler handler = handlers[registers.intNo];
		handler(registers);
		return;
	}
	write_char('\n');
	for (u8 i = 0; i < 80; i++) {
		write_char('=');
	}
	printf("\nReceived Unhandled Interrupt: %d\n", registers.intNo);
	printf("Error code: %b\n", registers.errCode);
	for (u8 i = 0; i < 80; i++) {
		write_char('=');
	}
	write_char('\n');
	halt();
}

/**
   Handle an IRQ interrupt
 */
void irqHandler(struct Registers registers) {
	// Reset PIC
	if (registers.intNo >= 40) {
		// Send reset signal to secondary
		io_out(0xA0, 0x20);
	}
	// Send reset signal to primary
	io_out(0x20, 0x20);

	// If there's a handler, call it.
	if (handlers[registers.intNo]) {
		Handler handler = handlers[registers.intNo];
		handler(registers);
	}
}
