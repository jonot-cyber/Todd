#include "keyboard.h"
#include "gdt.h"
#include "idt.h"
#include "isr.h"
#include "src/memory.h"
#include "src/monitor.h"
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
	Monitor::printf("Flags: %b\n", mboot->flags);
    Monitor::putString("Memory Information: ");
    outputPassFail(mboot->memPresent());
    if (mboot->memPresent()) {
	    Monitor::printf("\n| Lower Memory Limit: %dKB\n", mboot->memLower);
	    Monitor::printf("| Upper Memory Limit: %dKB\n", mboot->memUpper);
    }
    Monitor::putString("Boot Device: ");
    outputPassFail(mboot->bootDevicePresent());
    if (mboot->bootDevicePresent()) {
	    Monitor::printf("\n| Boot Device: 0x%x", mboot->bootDevice);
    }
    Monitor::putString("\nCMD Line: ");
    outputPassFail(mboot->cmdLinePresent());
    if (mboot->bootDevicePresent()) {
	    Monitor::printf("\n| Command Line: %s", mboot->cmdLine);
    }
    Monitor::putString("\nBoot Modules: ");
    outputPassFail(mboot->modsPresent());
    if (mboot->modsPresent()) {
	    Monitor::printf("\n| #: %d", mboot->modsCount);
        if (mboot->modsCount > 0) {
            halt();
        }
    }
    Monitor::putChar('\n');
}

extern "C" {
	int main(MultiBoot* mboot) {
		Monitor::init();
		multibootInfo(mboot);
		GDT::init();
		IDT::init();
	
		outputToddOS();
		Monitor::putChar('\n');

		Memory::init(); // initialize paging

		while (true) {}
	}
}
