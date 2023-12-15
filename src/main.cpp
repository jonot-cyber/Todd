#include "keyboard.h"
#include "gdt.h"
#include "idt.h"
#include "isr.h"
#include "timer.h"

void outputToddOS() {
    Monitor::putString("Welcome To ");
    Monitor::setBackgroundColor(VGAColor::Blue);
    Monitor::setForegroundColor(VGAColor::LightRed);
    Monitor::putChar('T');
    Monitor::setForegroundColor(VGAColor::LightBrown);
    Monitor::putChar('o');
    Monitor::setForegroundColor(VGAColor::LightGreen);
    Monitor::putChar('d');
    Monitor::setForegroundColor(VGAColor::LightCyan);
    Monitor::putChar('d');
    Monitor::setForegroundColor(VGAColor::LightBlue);
    Monitor::putChar('O');
    Monitor::setForegroundColor(VGAColor::LightMagenta);
    Monitor::putChar('S');

    Monitor::resetColor();
}

void outputHeader() {
    for (int i = 0; i < 80; i++) {
        Monitor::putChar('=');
    }
    Monitor::putChar('\n');
}

void outputPassFail(bool value) {
    Monitor::resetColor();
    if (value) {
        Monitor::setForegroundColor(VGAColor::LightGreen);
        Monitor::putString("PASS");
    } else {
        Monitor::setForegroundColor(VGAColor::LightRed);
        Monitor::putString("FAIL");
    }
    Monitor::resetColor();
}

void multibootInfo(MultiBoot* mboot) {
    Monitor::putString("Flags: ");
    Monitor::writeBin(mboot->flags);
    Monitor::putChar('\n');
    Monitor::putString("Memory Information: ");
    outputPassFail(mboot->memPresent());
    if (mboot->memPresent()) {
        Monitor::putString("\n| Lower Memory Limit: ");
        Monitor::writeDec(mboot->memLower);
        Monitor::putString("KB\n");
        Monitor::putString("| Upper Memory Limit: ");
        Monitor::writeDec(mboot->memUpper);
        Monitor::putString("KB\n");
    }
    Monitor::putString("Boot Device: ");
    outputPassFail(mboot->bootDevicePresent());
    if (mboot->bootDevicePresent()) {
        Monitor::putString("\n| Boot Device: ");
        Monitor::writeHex(mboot->bootDevice);
    }
    Monitor::putString("\nCMD Line: ");
    outputPassFail(mboot->cmdLinePresent());
    if (mboot->bootDevicePresent()) {
        Monitor::putString("\n| Command Line: ");
        Monitor::putString(reinterpret_cast<i8*>(mboot->cmdLine));
    }
    Monitor::putString("\nBoot Modules: ");
    outputPassFail(mboot->modsPresent());
    if (mboot->modsPresent()) {
        Monitor::putString("\n| #: ");
        Monitor::writeDec(mboot->modsCount);
        if (mboot->modsCount > 0) {
            halt();
        }
    }
    Monitor::putChar('\n');
}

extern "C" {
    /* Initial function */
    int main(MultiBoot* mboot) {
        Monitor::init();
        multibootInfo(mboot);
        GDT::init();
        Monitor::putString("GDT Initialized\n");
        IDT::init();
        Monitor::putString("IDT Initialized\n");
        Timer::init(100);
        asm volatile("sti"); // Enable interrupts

        PS2::init();
        Keyboard::init();
        Keyboard::setScanCodeSet(2);
        outputToddOS();
	Monitor::putChar('\n');
        while (true) {}
    }
}
