#include "serial.h"

#include "io_port.h"

/* COM1 port */
const u16 PORT = 0x3f8;

void serial_init() {
	/* All of this comes from https://wiki.osdev.org/ */
	
	/* Disable serial interrupts */
	io_out(PORT + 1, 0x00);

	/* Set the baud rate divisor */
	io_out(PORT + 3, 0x80);

	/* Set divisor to 3 */
	io_out(PORT + 0, 0x03);
	io_out(PORT + 1, 0x00);

	/* Some more settings stuff */
	io_out(PORT + 3, 0x03);

	/* Even more settings */
	io_out(PORT + 2, 0xC7);

	/* Enable interrupts */
	io_out(PORT + 4, 0x0B);

	/* Set to loopback mode to test */
	io_out(PORT + 4, 0x1E);
	io_out(PORT + 0, 0xAE);

	assert(io_in(PORT) == 0xAE, "serial_init: Port not working");

	/* Remove loopback mode */
	io_out(PORT + 4, 0x0F);
}

void _serial_write_char(i8 c) {
	/* Wait for the port to be ready */
	while ((io_in(PORT + 5) & 0x20) == 0);

	io_out(PORT, c);
}

void serial_write_char(i8 c) {
	/* Implement UNIX style line endings */
	if (c == '\n')
		_serial_write_char('\r');
	_serial_write_char(c);
}
