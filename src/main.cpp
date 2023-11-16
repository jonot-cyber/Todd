#include "monitor.h"
#include "ps2.h"

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
        outputHeader(m);
        outputToddOS(m);
        m << "\n  Version ";
        m.foregroundColor = VGAColor::LightBrown;
        m << "0.1";
        m.resetColor();
        m << '\n';
        outputHeader(m);

        PS2 ps2;
        m << "        PS2 Test: ";
        outputPassFail(m, ps2.isControllerWorking);
        m << "\nPS2 Dual Channel: ";
        outputPassFail(m, ps2.isDualChannel);
        m << "\n      PS2 Port 1: ";
        outputPassFail(m, ps2.isPort1Working);
        m << "\n       PS2 Reset: ";
        outputPassFail(m, ps2.isResetWorking);
        PS2DeviceType deviceId = ps2.identifyDevice();
        m << "\n       Device ID: ";
        m.writeHex((u8)deviceId);
    }
}