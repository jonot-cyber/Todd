#pragma once

#include "common.h"

volatile extern u32 ticks;

namespace Timer {
	void init(u32 frequency);
};
