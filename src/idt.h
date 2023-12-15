#pragma once

#include "common.h"

struct Registers {
    u32 ds;
    u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
    u32 intNo, errCode;
    u32 eip, cs, eflags, useresp, ss;
};

extern "C" {
    extern void IDTFlush(u32);
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

    void isrHandler(Registers);
    void irqHandler(Registers);
}

namespace IDT {

    enum Flags: u8 {
        Present = 0b10000000,
        Ring0 = 0b01100000,
        Ring1 = 0b01000000,
        Ring2 = 0b00100000,
        Ring3 = 0b0000000,
        Base = 0b00110,
    };

    struct Entry {
        u16 baseLow;
        u16 selector;
        u8 always0;
        Flags flags;
        u16 baseHigh;

        void set(u32, u16, Flags);
    } __attribute__((packed));

    struct Ptr {
        u16 limit;
        u32 base;
    } __attribute__((packed));

    void init();
    static void flush(u32 v);
};