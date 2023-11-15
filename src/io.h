#pragma once
#include "common.h"

class IO {
public:
    explicit IO(u16 port);

    void operator<<(u8 value) const;
    void operator>>(u8& value) const;
    void operator<<(u16 value) const;
    void operator>>(u16& value) const;
private:
    u16 port;
};