#include "keyboard.h"

#include "ps2.h"
#include "monitor.h"

static bool keys[126]{};
static bool isScanning = false;

void Keyboard::enableScanning() {
    isScanning = true;
    PS2::out(0xF4);
    PS2::in();
}

void Keyboard::disableScanning() {
    isScanning = false;
    PS2::out(0xF5);
    PS2::in();
}

void Keyboard::init() {
    disableScanning();
    for (bool& key : keys) {
        key = false;
    }
}

bool Keyboard::setLed(LEDState state) {
    disableScanning();
    PS2::out(0xED);
    PS2::out(static_cast<u8>(state));
    return PS2::in() == 0xFA;
}

bool Keyboard::echo() {
    disableScanning();
    PS2::out(0xEE);
    return PS2::in() == 0xEE;
}

void Keyboard::setScanCodeSet(i8 code) {
    disableScanning();
    PS2::out(0);
    if (PS2::in() != 0xFA) {
        return;
    }
    PS2::out(code);
    if (PS2::in() != 0xFA) {
        return;
    }
}

i8 Keyboard::getScanCodeSet() {
    disableScanning();
    PS2::out(0xF0);
    PS2::out(0);
    u8 ret = PS2::in();
    if (ret != 0xFA) {
        return -1;
    }
    switch (PS2::in()) {
        case 0x43:
            return 1;
        case 0x41:
            return 2;
        case 0x3f:
            return 3;
        default:
            return -2;
    }
}

void Keyboard::setKey(Key key, bool enabled) {
    keys[(u8)key] = enabled;
}

bool Keyboard::keyPressed(Key key) {
    return keys[(u8)key];
}

void Keyboard::test() {
    Monitor::setBackgroundColor(keys[(u8)Key::W] ? VGAColor::LightGreen : VGAColor::LightRed);
    Monitor::setPos(1, 1, 'W');
    Monitor::setBackgroundColor(keys[(u8)Key::A] ? VGAColor::LightGreen : VGAColor::LightRed);
    Monitor::setPos(0, 2, 'A');
    Monitor::setBackgroundColor(keys[(u8)Key::S] ? VGAColor::LightGreen : VGAColor::LightRed);
    Monitor::setPos(1, 2, 'S');
    Monitor::setBackgroundColor(keys[(u8)Key::D] ? VGAColor::LightGreen : VGAColor::LightRed);
    Monitor::setPos(2, 2, 'D');
}

Keyboard::Key getCode(u8 scan) {
    using namespace Keyboard;

    switch (scan) {
        case 0x01: return Key::F9;
        case 0x03: return Key::F5;
        case 0x04: return Key::F3;
        case 0x05: return Key::F1;
        case 0x06: return Key::F2;
        case 0x07: return Key::F12;
        case 0x09: return Key::F10;
        case 0x0A: return Key::F8;
        case 0x0B: return Key::F6;
        case 0x0C: return Key::F4;
        case 0x0D: return Key::Tab;
        case 0x0E: return Key::BackTick;
        case 0x11: return Key::LeftAlt;
        case 0x12: return Key::LeftShift;
        case 0x14: return Key::LeftControl;
        case 0x15: return Key::Q;
        case 0x16: return Key::One;
        case 0x1A: return Key::Z;
        case 0x1B: return Key::S;
        case 0x1C: return Key::A;
        case 0x1D: return Key::W;
        case 0x1E: return Key::Two;
        case 0x21: return Key::C;
        case 0x22: return Key::X;
        case 0x23: return Key::D;
        case 0x24: return Key::E;
        case 0x25: return Key::Four;
        case 0x26: return Key::Three;
        case 0x29: return Key::Space;
        case 0x2A: return Key::V;
        case 0x2B: return Key::F;
        case 0x2C: return Key::T;
        case 0x2D: return Key::R;
        case 0x2E: return Key::Five;
        case 0x31: return Key::N;
        case 0x32: return Key::B;
        case 0x33: return Key::H;
        case 0x35: return Key::Y;
        case 0x3A: return Key::M;
        case 0x3B: return Key::J;
        case 0x3C: return Key::U;
        case 0x3D: return Key::Seven;
        case 0x3E: return Key::Eight;
        case 0x41: return Key::Comma;
        case 0x42: return Key::K;
        case 0x43: return Key::I;
        case 0x44: return Key::O;
        case 0x45: return Key::Zero;
        case 0x46: return Key::Nine;
        case 0x49: return Key::Period;
        case 0x4A: return Key::Slash;
        case 0x4B: return Key::L;
        case 0x4C: return Key::SemiColon;
        case 0x4D: return Key::P;
        case 0x4E: return Key::Minus;
        case 0x52: return Key::Apostrophe;
        case 0x54: return Key::LeftBracket;
        case 0x55: return Key::Equal;
        case 0x58: return Key::CapsLock;
        case 0x59: return Key::RightShift;
        case 0x5A: return Key::Enter;
        case 0x5B: return Key::RightBracket;
        case 0x5D: return Key::BackSlash;
        case 0x66: return Key::BackSpace;
        case 0x69: return Key::Num1;
        case 0x6B: return Key::Num4;
        case 0x6C: return Key::Num7;
        case 0x70: return Key::Num0;
        case 0x71: return Key::NumPeriod;
        case 0x72: return Key::Num2;
        case 0x73: return Key::Num5;
        case 0x74: return Key::Num6;
        case 0x75: return Key::Num8;
        case 0x76: return Key::Escape;
        case 0x77: return Key::NumLock;
        case 0x78: return Key::F11;
        case 0x79: return Key::NumPlus;
        case 0x7A: return Key::Num3;
        case 0x7B: return Key::NumMinus;
        case 0x7C: return Key::NumAsterisk;
        case 0x7D: return Key::Num9;
        case 0x7E: return Key::ScrollLock;
        case 0x83: return Key::F7;
    }
    return Key::WWWSearch;
}

Keyboard::Key getE0Code(u8 code) {
    using namespace Keyboard;

    switch (code) {
        case 0x10: return Key::WWWSearch;
        case 0x11: return Key::RightAlt;
        case 0x14: return Key::RightControl;
        case 0x15: return Key::PreviousTrack;
        case 0x18: return Key::WWWFavorites;
        case 0x1F: return Key::LeftSuper;
        case 0x20: return Key::WWWRefresh;
        case 0x21: return Key::VolumeDown;
        case 0x23: return Key::Mute;
        case 0x27: return Key::RightSuper;
        case 0x28: return Key::WWWStop;
        case 0x2B: return Key::Calculator;
        case 0x2F: return Key::Apps;
        case 0x30: return Key::WWWForward;
        case 0x32: return Key::VolumeUp;
        case 0x34: return Key::PlayPause;
        case 0x37: return Key::Power;
        case 0x38: return Key::WWWBack;
        case 0x3A: return Key::WWWHome;
        case 0x3B: return Key::Stop;
        case 0x3F: return Key::Sleep;
        case 0x40: return Key::MyComputer;
        case 0x48: return Key::Email;
        case 0x4A: return Key::NumSlash;
        case 0x4D: return Key::NextTrack;
        case 0x50: return Key::MediaSelect;
        case 0x5A: return Key::NumEnter;
        case 0x5E: return Key::Wake;
        case 0x69: return Key::End;
        case 0x6B: return Key::Left;
        case 0x6C: return Key::Home;
        case 0x70: return Key::Insert;
        case 0x71: return Key::Delete;
        case 0x72: return Key::Down;
        case 0x74: return Key::Right;
        case 0x75: return Key::Up;
        case 0x7A: return Key::PageDown;
        case 0x7D: return Key::PageUp;
    }
    return Key::WWWSearch;
}

void Keyboard::scan() {
    enableScanning();
    u8 ret = PS2::in();
    bool enabled = true;
    Key code;
    if (ret == 0xE0) {
        ret = PS2::in();
        if (ret == 0xF0) {
            ret = PS2::in();
            enabled = false;
        }
        code = getE0Code(ret);
    } else if (ret == 0xF0) {
        ret = PS2::in();
        code = getCode(ret);
        enabled = false;
    } else {
        code = getCode(ret);
    }
    setKey(code, enabled);
    PS2::in();
}
