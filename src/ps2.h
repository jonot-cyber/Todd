#pragma once

#include "io.h"

namespace PS2 {
    constexpr u16 cmdPort = 0x64;
    constexpr u16 dataPort = 0x60;

    enum class Command : u8 {
        ReadByte0 = 0x20,
        ReadByteN = 0x21,
        WriteByte0 = 0x60,
        WriteByteN = 0x61,
        DisablePort2 = 0xA7,
        EnablePort2 = 0xA8,
        TestPort2 = 0xA9,
        TestController = 0xAA,
        TestPort1 = 0xAB,
        DiagnosticDump = 0xAC,
        DisablePort1 = 0xAD,
        EnablePort1 = 0xAE,
        ReadControllerInput = 0xC0,
        CopyLowInputBitsToStatus = 0xC1,
        CopyHighInputBitsToStatus = 0xC2,
        ReadControllerOutput = 0xD0,
        WriteByteToOutput = 0xD1,
        WriteByteToOutput1 = 0xD2,
        WriteByteToOutput2 = 0xD3,
        WriteByteToInput2 = 0xD4,
        PulseOutput = 0xF0,
    };

    enum class TestPortResult : u8 {
        Pass = 0,
        ClockStuckLow = 1,
        ClockStuckHigh = 2,
        DataStuckLow = 3,
        DataStuckHigh = 4,
    };

    enum class TestResult : u8 {
        Pass = 0x55,
        Fail = 0xFC,
    };

    enum class ResetResponse : u8 {
        Success = 0xFA,
        Failure = 0xFC,
    };

    enum class DeviceType : u8 {
        None,
        ATKeyboard,
        Mouse,
        MouseWithScrollWheel,
        Mouse5Button,
        MF2Keyboard,
        ShortKeyboard,
        N97Keyboard,
        HostConnectedKeyboard,
        Keyboard,
        JapanGKeyboard,
        JapanPKeyboard,
        JapanAKeyboard,
        SunKeyboard,
    };

    void init();

    DeviceType identifyDevice();

    void out(u8);
    u8 in();

    bool test();
    void enablePort(bool first = true);
    void disablePort(bool first = true);
}