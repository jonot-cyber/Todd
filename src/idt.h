#ifndef IDT_H
#define IDT_H

#include "common.h"

/**
   A structure of CPU registers
*/
struct Registers {
	u32 ds;
	u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
	u32 intNo, errCode;
	u32 eip, cs, eflags, useresp, ss;
};

/**
   Flush IDT information to the CPU
 */
extern void IDTFlush(u32);

// A bunch of interrupt handlers. These are defined with assembly
// macros from the internet
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

/**
   Handles an interrupt
 */
void isrHandler(struct Registers);

/**
   This also handles an interrupt
*/
void irqHandler(struct Registers);

/**
   The flags of an IDT entry
 */
enum IDTFlags {
	PRESENT = 0b10000000,
	RING_0 = 0b01100000,
	RING_1 = 0b01000000,
	RING_2 = 0b00100000,
	RING_3 = 0b00000000,
	BASE = 0b00110,
};

/**
   An IDT entry. This stores information about an interrupt
   handler. It is also a structure that makes you hate Intel™
 */
struct IDTEntry {
	u16 base_low;
	u16 selector;
	u8 always0;
	u8 flags;
	u16 base_high;
} __attribute__((packed));

/**
  Set an IDT entry to the values we want
 */
void set_idt_entry(struct IDTEntry*, u32, u16, enum IDTFlags);

struct IDTPointer {
	u16 limit;
	u32 base;
} __attribute__((packed));

/**
   Initialize the idt
 */
void idt_init();

#endif
