#include "monitor.h"
#include "ps2.h"
#include "keyboard.h"

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

void outputPassFail(Monitor& m, bool value) {
    m.resetColor();
    if (value) {
        m.foregroundColor = VGAColor::LightGreen;
        m << "PASS";
    } else {
        m.foregroundColor = VGAColor::LightRed;
        m << "FAIL";
    }
    m.resetColor();
}

extern "C" {
    /* Initial function */
    int main(void* mboot) {
        Monitor m;
        Keyboard k;
        while (true) {
            k.scan();
            k.test(m);
        }
    }
}