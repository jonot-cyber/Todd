#pragma once

#include "common.h"

/**
   Calculate a crc32 hash

   @param data is the data to hash
   @param len is how long the data is.
 */
u32 crc32(const u8* data, u32 len);

namespace Types {
};
