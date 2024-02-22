#include "ps2.h"

#include "io_port.h"

static bool isDualChannel = false;
static bool isResetWorking = false;

const u16 CMD_PORT = 0x64;
const u16 DATA_PORT = 0x60;

void disable_port(bool);
void enable_port(bool);

void ps2_init() {
	// TODO: Currently we assume that this exists, which isn't true for some niche devices

	// Disable all devices
	disable_port(true);
	disable_port(false);

	// Flush output buffer
	io_in(CMD_PORT);

	// Set Controller Configuration Byte
	io_out(CMD_PORT, 0x20);
	u8 ret = io_in(DATA_PORT);
	isDualChannel = (ret & 0x10) == 0;
	// TODO: Get this working without translation
	ret = (ret & 0xBF) | 0; // Disable translation
	io_out(CMD_PORT, 0x60);
	io_out(DATA_PORT, ret);

	// Enable devices
	enable_port(false);
	enable_port(true);

	// Reset devices
	io_out(DATA_PORT, 0xFF);
	u8 resetResponse = io_in(DATA_PORT);
	isResetWorking = resetResponse == 0xFA;
}

void enable_port(bool first) {
	if (first) {
		io_out(CMD_PORT, 0xAE);
	} else {
		io_out(CMD_PORT, 0xA8);
	}
}

void disable_port(bool first) {
	if (first) {
		io_out(CMD_PORT, 0xAD);
	} else {
		io_out(CMD_PORT, 0xA7);
	}
}

void ps2_out(u8 dat) {
	io_out(DATA_PORT, dat);
}

u8 ps2_in() {
	return io_in(DATA_PORT);
}
