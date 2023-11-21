#pragma once

#include "common.h"

extern "C" {
    void GDTFlush(u32);
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

    static void init();
}