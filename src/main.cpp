#include "monitor.h"

void outputToddOS(Monitor& m) {
    m << "Welcome To ";
    m.backgroundColor = VGAColor::Blue;
    m.foregroundColor = VGAColor::LightRed;
    m << 'T';
    m.foregroundColor = VGAColor::LightBrown;
    m << 'o';
    m.foregroundColor = VGAColor::LightGreen;
    m << 'd';
    m.foregroundColor = VGAColor::LightCyan;
    m << 'd';
    m.foregroundColor = VGAColor::LightBlue;
    m << 'O';
    m.foregroundColor = VGAColor::LightMagenta;
    m << 'S';
    m.resetColor();
}

void outputHeader(Monitor& monitor) {
    for (int i = 0; i < 80; i++) {
        monitor << '=';
    }
    monitor << '\n';
}

extern "C" {
    /* Initial function */
    int main(void* mboot) {
        Monitor m;
        outputHeader(m);
        outputToddOS(m);
        m << "\n  Version ";
        m.foregroundColor = VGAColor::LightBrown;
        m << "0.1";
        m.resetColor();
        m << '\n';
        outputHeader(m);
        m << "This is an alpha version of the ToddOS operating system. It currently is able to boot with a multiboot compatible boot loader, and can output text to a VGA screen. Nothing else.";
        for (int i = 0; i < 20; i++) {
            m << '\n';
        }
        for (int i = 0; i < 77; i++) {
            m << ' ';
        }
        m.foregroundColor = VGAColor::LightMagenta;
        m << "OwO";
    }
}