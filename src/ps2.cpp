#include "ps2.h"

PS2::PS2() : cmdPort(IO(0x64)), dataPort(IO(0x60)) {
    // TODO: Currently we assume that this exists, which isn't true for some niche devices

    // Disable all devices
    disablePort(true);
    disablePort(false);

    // Flush output buffer
    u8 nil;
    cmdPort >> nil;

    // Set Controller Configuration Byte
    cmdPort << (u8)PS2Command::ReadByte0;
    u8 ret;
    dataPort >> ret;
    isDualChannel = (ret & 0x10) == 0;
    // TODO: Get this working without translation
    // ret = (ret & 0xBF) | 0; // Disable translation
    cmdPort << (u8)PS2Command::WriteByte0;
    dataPort << ret;

    // Perform self test
    isControllerWorking = test();
    if (!isControllerWorking) {
        return;
    }

    // Perform interface test
    cmdPort << (u8)PS2Command::TestPort1;
    PS2TestPortResult status = PS2TestPortResult::Pass;
    dataPort >> *(u8*)&status;
    isPort1Working = status == PS2TestPortResult::Pass;
    // TODO: Test second port if it exists

    // Enable devices
    enablePort(false);
    enablePort(true);

    // Reset devices
    dataPort << (u8)0xFF;
    PS2ResetResponse resetResponse;
    dataPort >> *(u8*)&resetResponse;
    isResetWorking = resetResponse == PS2ResetResponse::Success;
}

void PS2::enablePort(bool first) {
    if (first) {
        cmdPort << (u8)PS2Command::EnablePort1;
    } else {
        cmdPort << (u8)PS2Command::EnablePort2;
    }
}

void PS2::disablePort(bool first) {
    if (first) {
        cmdPort << (u8)PS2Command::DisablePort1;
    } else {
        cmdPort << (u8)PS2Command::DisablePort2;
    }
}

bool PS2::test() {
    cmdPort << (u8)PS2Command::TestController;
    PS2TestResult res = PS2TestResult::Fail;
    dataPort >> *(u8*)&res;
    return res == PS2TestResult::Pass;
}

PS2DeviceType PS2::identifyDevice() {
    u8 ret;
    dataPort << (u8)0xF5; // Disable scanning command
    dataPort >> ret;
    if (ret != 0XFA) {
        dataPort << (u8)0xF4; // Enable scanning
        return PS2DeviceType::None;
    }
    dataPort << (u8)0xF2; // Identify command
    dataPort >> ret;
    if (ret != 0xFA) {
        dataPort << (u8)0xF4; // Enable scanning
        return PS2DeviceType::None;
    }
    dataPort >> ret;
    switch (ret) {
        case 0x00:
            dataPort << (u8)0xF4; // Enable scanning
            return PS2DeviceType::ATKeyboard;
        case 0x03:
            dataPort << (u8)0xF4; // Enable scanning
            return PS2DeviceType::Mouse;
        case 0x04:
            dataPort << (u8)0xF4; // Enable scanning
            return PS2DeviceType::Mouse5Button;
        case 0xAB:
            dataPort >> ret;
            dataPort << (u8)0xF4; // Enable scanning
            switch (ret) {
                case 0x41:
                case 0xC1:
                    return PS2DeviceType::MF2Keyboard;
                case 0x54:
                    return PS2DeviceType::ShortKeyboard;
                case 0x85:
                    return PS2DeviceType::N97Keyboard;
                case 0x86:
                    return PS2DeviceType::Keyboard;
                case 0x90:
                    return PS2DeviceType::JapanGKeyboard;
                case 0x91:
                    return PS2DeviceType::JapanPKeyboard;
                case 0x92:
                    return PS2DeviceType::JapanAKeyboard;
                case 0xA1:
                    return PS2DeviceType::SunKeyboard;
                default:
                    return PS2DeviceType::None;
            }
        default:
            return PS2DeviceType::None;
    }
}

PS2 PS2::operator<<(u8 dat) {
    dataPort << dat;
    return *this;
}

PS2 PS2::operator>>(u8& dat) {
    dataPort >> dat;
    return *this;
}