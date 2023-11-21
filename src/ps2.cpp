#include "ps2.h"
#include "monitor.h"

static bool isDualChannel = false;
static bool isControllerWorking = false;
static bool isPort1Working = false;
static bool isResetWorking = false;

void PS2::init() {
    // TODO: Currently we assume that this exists, which isn't true for some niche devices

    // Disable all devices
    disablePort(true);
    disablePort(false);

    // Flush output buffer
    IO::in(0x64);

    // Set Controller Configuration Byte
    IO::out(cmdPort, static_cast<u8>(Command::ReadByte0));
    u8 ret = IO::in(dataPort);
    isDualChannel = (ret & 0x10) == 0;
    // TODO: Get this working without translation
    ret = (ret & 0xBF) | 0; // Disable translation
    IO::out(cmdPort, static_cast<u8>(Command::WriteByte0));
    IO::out(dataPort, ret);

    // Perform self test
    isControllerWorking = test();
    if (!isControllerWorking) {
        return;
    }

    // Perform interface test
    IO::out(cmdPort, static_cast<u8>(Command::TestPort1));
    auto status = static_cast<TestPortResult>(IO::in(dataPort));
    isPort1Working = status == TestPortResult::Pass;
    // TODO: Test second port if it exists

    // Enable devices
    enablePort(false);
    enablePort(true);

    // Reset devices
    IO::out(dataPort, 0xFF);
    auto resetResponse = static_cast<ResetResponse>(IO::in(dataPort));
    isResetWorking = resetResponse == ResetResponse::Success;
}

void PS2::enablePort(bool first) {
    if (first) {
        IO::out(cmdPort, static_cast<u8>(Command::EnablePort1));
    } else {
        IO::out(cmdPort, static_cast<u8>(Command::EnablePort2));
    }
}

void PS2::disablePort(bool first) {
    if (first) {
        IO::out(cmdPort, static_cast<u8>(Command::DisablePort1));
    } else {
        IO::out(cmdPort, static_cast<u8>(Command::DisablePort2));
    }
}

bool PS2::test() {
    IO::out(cmdPort, static_cast<u8>(Command::TestController));
    auto res = static_cast<TestResult>(IO::in(dataPort));
    return res == TestResult::Pass;
}

PS2::DeviceType PS2::identifyDevice() {
    IO::out(dataPort, 0xF5); // Disable scanning
    u8 ret = IO::in(dataPort);
    if (ret != 0XFA) {
        IO::out(dataPort, 0xF4); // Enable scanning
        return DeviceType::None;
    }
    IO::out(dataPort, 0xF2); // Identify command
    ret = IO::in(dataPort);
    if (ret != 0xFA) {
        IO::out(dataPort, 0xF4);
        return DeviceType::None;
    }
    ret = IO::in(dataPort);
    switch (ret) {
        case 0x00:
            IO::out(dataPort, 0xF4);
            return DeviceType::Mouse;
        case 0x03:
            IO::out(dataPort, 0xF4);
            return DeviceType::MouseWithScrollWheel;
        case 0x04:
            IO::out(dataPort, 0xF4);
            return DeviceType::Mouse5Button;
        case 0xAB:
            ret = IO::in(dataPort);
            IO::out(dataPort, 0xF4);
            switch (ret) {
                case 0x83:
                case 0xC1:
                    return DeviceType::MF2Keyboard;
                case 0x84:
                    return DeviceType::ShortKeyboard;
                case 0x85:
                    return DeviceType::N97Keyboard;
                case 0x86:
                    return DeviceType::Keyboard;
                case 0x90:
                    return DeviceType::JapanGKeyboard;
                case 0x91:
                    return DeviceType::JapanPKeyboard;
                case 0x92:
                    return DeviceType::JapanAKeyboard;
                case 0xA1:
                    return DeviceType::SunKeyboard;
                default:
                    return DeviceType::None;
            }
        default:
            return DeviceType::None;
    }
}

void PS2::out(u8 dat) {
    IO::out(dataPort, dat);
}
u8 PS2::in() {
    return IO::in(dataPort);
}