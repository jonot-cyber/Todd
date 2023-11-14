#pragma once
#include "common.h"

class IO {
public:
    IO(u16 port);

    void operator<<(u8 value);
    void operator>>(u8& value);
    void operator<<(u16 value);
    void operator>>(u16& value);
private:
    u16 port;
};