#pragma once

#include "common.h"

namespace IO {
    void out(u16 port, u8 value);
    u8 in(u16 port);
    u16 inWord(u16 port);
};