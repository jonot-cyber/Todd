#pragma once

#include "common.h"
extern "C" {
    void GDTFlush(u32);
    void IDTFlush(u32);
    void isr0();
    void isr1();
    void isr2();
    void isr3();
    void isr4();
    void isr5();
    void isr6();
    void isr7();
    void isr8();
    void isr9();
void isr10();
void isr11();
void isr12();
void isr13();
void isr14();
void isr15();
void isr16();
void isr17();
void isr18();
void isr19();
void isr20();
void isr21();
void isr22();
void isr23();
void isr24();
void isr25();
void isr26();
void isr27();
void isr28();
void isr29();
void isr30();
void isr31();
}

namespace GDT {
    enum AccessByte : u8 {
        Present = 0b10000000,
        Ring0 = 0b00000000,
        Ring1 = 0b00100000,
        Ring2 = 0b01000000,
        Ring3 = 0b01100000,
        DescriptorType = 0b00010000,
    };

    enum GranularityByte : u8 {
        Granularity1 = 0b00000000,
        Granularity1k = 0b10000000,
        Operand16 = 0b00000000,
        Operand32 = 0b01000000,
    };

    struct Entry {
        u16 limitLow;
        u16 baseLow;
        u8 baseMiddle;
        u8 access;
        u8 granularity;
        u8 baseHigh;

        void set(u32, u32, AccessByte, GranularityByte);
    } __attribute__((packed));

    struct Ptr {
        u16 limit;
        u32 base;
    } __attribute__((packed));

    void flush(u32);

    void init();
}