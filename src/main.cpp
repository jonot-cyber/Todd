#include "gdt.h"
#include "idt.h"
#include "memory.h"
#include "monitor.h"

void outputToddOS() {
    Monitor::writeString("Welcome To ");
    Monitor::setBackgroundColor(VGAColor::Blue);
    Monitor::setForegroundColor(VGAColor::LightRed);
    Monitor::writeChar('T');
    Monitor::setForegroundColor(VGAColor::LightBrown);
    Monitor::writeChar('o');
    Monitor::setForegroundColor(VGAColor::LightGreen);
    Monitor::writeChar('d');
    Monitor::setForegroundColor(VGAColor::LightCyan);
    Monitor::writeChar('d');
    Monitor::setForegroundColor(VGAColor::LightBlue);
    Monitor::writeChar('O');
    Monitor::setForegroundColor(VGAColor::LightMagenta);
    Monitor::writeChar('S');

    Monitor::resetColor();
}

void outputHeader() {
    for (int i = 0; i < 80; i++) {
        Monitor::writeChar('=');
    }
    Monitor::writeChar('\n');
}

void outputPassFail(bool value) {
	
    Monitor::resetColor();
    if (value) {
        Monitor::setForegroundColor(VGAColor::LightGreen);
        Monitor::writeString("PASS");
    } else {
        Monitor::setForegroundColor(VGAColor::LightRed);
        Monitor::writeString("FAIL");
    }
    Monitor::resetColor();
}

void multibootInfo(MultiBoot* mboot) {
	Monitor::printf("Flags: %b\n", mboot->flags);
    Monitor::writeString("Memory Information: ");
    outputPassFail(mboot->memPresent());
    if (mboot->memPresent()) {
	    Monitor::printf("\n| Lower Memory Limit: %dKB\n", mboot->memLower);
	    Monitor::printf("| Upper Memory Limit: %dKB\n", mboot->memUpper);
    }
    Monitor::writeString("Boot Device: ");
    outputPassFail(mboot->bootDevicePresent());
    if (mboot->bootDevicePresent()) {
	    Monitor::printf("\n| Boot Device: 0x%x", mboot->bootDevice);
    }
    Monitor::writeString("\nCMD Line: ");
    outputPassFail(mboot->cmdLinePresent());
    if (mboot->bootDevicePresent()) {
	    Monitor::printf("\n| Command Line: %s", mboot->cmdLine);
    }
    Monitor::writeString("\nBoot Modules: ");
    outputPassFail(mboot->modsPresent());
    if (mboot->modsPresent()) {
	    Monitor::printf("\n| #: %d", mboot->modsCount);
        if (mboot->modsCount > 0) {
            halt();
        }
    }
    Monitor::writeChar('\n');
}

extern "C" {
	int main(MultiBoot* mboot) {
		Monitor::init();
		multibootInfo(mboot);
		GDT::init();
		IDT::init();
	
		outputToddOS();
		Monitor::writeChar('\n');

		Memory::init(); // initialize paging
		u32 a = Memory::kmalloc(8);
		u32 b = Memory::kmalloc(8);
		u32 c = Memory::kmalloc(8);
		Monitor::printf("a: 0x%x, b: 0x%x, c: 0x%x.\n", a, b, c);
		Memory::kfree((void*)b);
		Memory::kfree((void*)c);
		u32 d = Memory::kmalloc(8);
		Monitor::printf("d: 0x%x\n", d);

		while (true) {}
	}
}
