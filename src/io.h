#pragma once

#include "common.h"

namespace IO {
	/**
	   Write a 8 bit value to a port

	   @param port is the port to write to
	   @param value is the value to write
	 */
	void out(u16 port, u8 value);

	/**
	   Read a 8 bit value from a port

	   @param port is the port to read from
	*/
	u8 in(u16 port);

	/**
	   Read a 16 bit value from a port

	   @param port is the port to read from
	*/
	u16 inWord(u16 port);
};
