#include "keyboard.h"

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

extern "C" {
    /* Initial function */
    int main(void* mboot) {
        Monitor::init();
        PS2::init();
        Keyboard::init();
        Keyboard::setScanCodeSet(2);
        outputToddOS();
        while (true) {
            Keyboard::scan();
            Keyboard::test();
        }
    }
}