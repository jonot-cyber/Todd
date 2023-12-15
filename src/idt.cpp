#include "idt.h"
#include "isr.h"
#include "monitor.h"

const u16 PRIMARY_CMD = 0x20;
const u16 PRIMARY_DATA = 0x21;
const u16 SECONDARY_CMD = 0xA0;
const u16 SECONDARY_DATA = 0xA1;

static IDT::Entry entries[256];
static IDT::Ptr ptr;

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

    memset(reinterpret_cast<u8 *>(&entries), 0, sizeof(Entry) * 256);

    entries[0].set(reinterpret_cast<u32>(isr0), 0x08, static_cast<Flags>(0x8E));
    entries[1].set(reinterpret_cast<u32>(isr1), 0x08, static_cast<Flags>(0x8E));
    entries[2].set(reinterpret_cast<u32>(isr2), 0x08, static_cast<Flags>(0x8E));
    entries[3].set(reinterpret_cast<u32>(isr3), 0x08, static_cast<Flags>(0x8E));
    entries[4].set(reinterpret_cast<u32>(isr4), 0x08, static_cast<Flags>(0x8E));
    entries[5].set(reinterpret_cast<u32>(isr5), 0x08, static_cast<Flags>(0x8E));
    entries[6].set(reinterpret_cast<u32>(isr6), 0x08, static_cast<Flags>(0x8E));
    entries[7].set(reinterpret_cast<u32>(isr7), 0x08, static_cast<Flags>(0x8E));
    entries[8].set(reinterpret_cast<u32>(isr8), 0x08, static_cast<Flags>(0x8E));
    entries[9].set(reinterpret_cast<u32>(isr9), 0x08, static_cast<Flags>(0x8E));
    entries[10].set(reinterpret_cast<u32>(isr10), 0x08, static_cast<Flags>(0x8E));
    entries[11].set(reinterpret_cast<u32>(isr11), 0x08, static_cast<Flags>(0x8E));
    entries[12].set(reinterpret_cast<u32>(isr12), 0x08, static_cast<Flags>(0x8E));
    entries[13].set(reinterpret_cast<u32>(isr13), 0x08, static_cast<Flags>(0x8E));
    entries[14].set(reinterpret_cast<u32>(isr14), 0x08, static_cast<Flags>(0x8E));
    entries[15].set(reinterpret_cast<u32>(isr15), 0x08, static_cast<Flags>(0x8E));
    entries[16].set(reinterpret_cast<u32>(isr16), 0x08, static_cast<Flags>(0x8E));
    entries[17].set(reinterpret_cast<u32>(isr17), 0x08, static_cast<Flags>(0x8E));
    entries[18].set(reinterpret_cast<u32>(isr18), 0x08, static_cast<Flags>(0x8E));
    entries[19].set(reinterpret_cast<u32>(isr19), 0x08, static_cast<Flags>(0x8E));
    entries[20].set(reinterpret_cast<u32>(isr20), 0x08, static_cast<Flags>(0x8E));
    entries[21].set(reinterpret_cast<u32>(isr21), 0x08, static_cast<Flags>(0x8E));
    entries[22].set(reinterpret_cast<u32>(isr22), 0x08, static_cast<Flags>(0x8E));
    entries[23].set(reinterpret_cast<u32>(isr23), 0x08, static_cast<Flags>(0x8E));
    entries[24].set(reinterpret_cast<u32>(isr24), 0x08, static_cast<Flags>(0x8E));
    entries[25].set(reinterpret_cast<u32>(isr25), 0x08, static_cast<Flags>(0x8E));
    entries[26].set(reinterpret_cast<u32>(isr26), 0x08, static_cast<Flags>(0x8E));
    entries[27].set(reinterpret_cast<u32>(isr27), 0x08, static_cast<Flags>(0x8E));
    entries[28].set(reinterpret_cast<u32>(isr28), 0x08, static_cast<Flags>(0x8E));
    entries[29].set(reinterpret_cast<u32>(isr29), 0x08, static_cast<Flags>(0x8E));
    entries[30].set(reinterpret_cast<u32>(isr30), 0x08, static_cast<Flags>(0x8E));
    entries[31].set(reinterpret_cast<u32>(isr31), 0x08, static_cast<Flags>(0x8E));

    remapIRQTable();
    entries[32].set(reinterpret_cast<u32>(irq0), 0x08, static_cast<Flags>(0x8E));
    entries[33].set(reinterpret_cast<u32>(irq1), 0x08, static_cast<Flags>(0x8E));
    entries[34].set(reinterpret_cast<u32>(irq2), 0x08, static_cast<Flags>(0x8E));
    entries[35].set(reinterpret_cast<u32>(irq3), 0x08, static_cast<Flags>(0x8E));
    entries[36].set(reinterpret_cast<u32>(irq4), 0x08, static_cast<Flags>(0x8E));
    entries[37].set(reinterpret_cast<u32>(irq5), 0x08, static_cast<Flags>(0x8E));
    entries[38].set(reinterpret_cast<u32>(irq6), 0x08, static_cast<Flags>(0x8E));
    entries[39].set(reinterpret_cast<u32>(irq7), 0x08, static_cast<Flags>(0x8E));
    entries[40].set(reinterpret_cast<u32>(irq8), 0x08, static_cast<Flags>(0x8E));
    entries[41].set(reinterpret_cast<u32>(irq9), 0x08, static_cast<Flags>(0x8E));
    entries[42].set(reinterpret_cast<u32>(irq10), 0x08, static_cast<Flags>(0x8E));
    entries[43].set(reinterpret_cast<u32>(irq11), 0x08, static_cast<Flags>(0x8E));
    entries[44].set(reinterpret_cast<u32>(irq12), 0x08, static_cast<Flags>(0x8E));
    entries[45].set(reinterpret_cast<u32>(irq13), 0x08, static_cast<Flags>(0x8E));
    entries[46].set(reinterpret_cast<u32>(irq14), 0x08, static_cast<Flags>(0x8E));
    entries[47].set(reinterpret_cast<u32>(irq15), 0x08, static_cast<Flags>(0x8E));

    flush(reinterpret_cast<u32>(&ptr));
}

void IDT::Entry::set(u32 base, u16 sel, IDT::Flags f) {
    baseLow = base & 0xFFFF;
    baseHigh = (base >> 16) & 0xFFFF;

    selector = sel;
    always0 = 0;
    flags = f;
}

void IDT::flush(u32 v) {
    IDTFlush(v);
}

void isrHandler(Registers registers) {
    Monitor::putChar('\n');
    for (u8 i = 0; i < 80; i++) {
        Monitor::putChar('=');
    }
    Monitor::putChar('\n');
    Monitor::putString("Received Interrupt: ");
    Monitor::writeDec(registers.intNo);
    Monitor::putChar('\n');
    for (u8 i = 0; i < 80; i++) {
        Monitor::putChar('=');
    }
    Monitor::putChar('\n');
}

void irqHandler(Registers registers) {
    if (registers.intNo >= 40) {
        // Send reset signal to secondary
        IO::out(0xA0, 0x20);
    }
    // Send reset signal to primary
    IO::out(0x20, 0x20);

    if (ISR::handlerPresent(registers.intNo)) {
        ISR::Handler handler = ISR::getHandler(registers.intNo);
        handler(registers);
    }
}