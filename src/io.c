#include "io.h"

#include "monitor.h"
#include "mutex.h"
#include "queue.h"
#include "serial.h"

static mutex_t io_lock = 0;
static struct Queue io_queue;

static bool MONITOR_BACKEND = false;
static bool SERIAL_BACKEND = false;

void io_init(bool enable_monitor, bool enable_serial) {
	if (enable_monitor) {
		MONITOR_BACKEND = true;
		monitor_init();
	}
	if (enable_serial) {
		SERIAL_BACKEND = true;
		serial_init();
	}
	queue_init(&io_queue, 256);
}

void _write_char(i8 c) {
	if (MONITOR_BACKEND) {
		monitor_write_char(c);
	}
	if (SERIAL_BACKEND) {
		serial_write_char(c);
	}
}

void write_char(i8 c) {
	/* If we can't lock, push to the queue */
	if (!try_lock(&io_lock)) {
		bool inserted = queue_push(&io_queue, c);
		if (inserted) {
			return;
		}
		/* If the queue is full, wait on the lock */
		lock(&io_lock);
	}
	/* Free the queue */
	while (io_queue.size) {
		/* We can safely dereference. If the queue was empty,
		 * the loop would end. */
		i8 value = *queue_pop(&io_queue) & 0xFF;
		_write_char(value);
	}
	_write_char(c);
	unlock(&io_lock);
}

void write_string(const i8* str) {
	while (*str) {
		write_char(*str);
		str++;
	}
}

void write_bin(u32 v) {
	for (u32 i = 0; i < 32; i++) {
		u8 digit = (v >> (31-i)) & 1;
		write_char('0' + digit);
	}
}

void write_dec(u32 v) {
	if (v == 0) {
		return;
	}
	write_dec(v / 10);
	write_char('0' + (v % 10));
}

void write_hex(u8 c) {
	const i8 hex_digits[] = {
		'0',
		'1',
		'2',
		'3',
		'4',
		'5',
		'6',
		'7',
		'8',
		'9',
		'A',
		'B',
		'C',
		'D',
		'E',
		'F',
	};
	u8 hi = c >> 4;
	u8 lo = c & 0xF;
	write_char(hex_digits[hi]);
	write_char(hex_digits[lo]);
}

void write_hex16(u16 c) {
	u8 hi = c >> 8;
	u8 lo = c;
	write_hex(hi);
	write_hex(lo);
}

void write_hex32(u32 c) {
	u16 hi = c >> 16;
	u16 lo = c;
	write_hex16(hi);
	write_hex16(lo);
}

u8 hex2num(i8 c) {
	if (c >= '0' && c <= '9') {
		return c - '0';
	}
	if (c >= 'A' && c <= 'F') {
		return c - 'A' + 10;
	}
	if (c >= 'a' && c <= 'f') {
		return c - 'a' + 10;
	}
	assert(false, "hex2num: Invalid hex digit");
}

void printf(const i8* str, ...) {
	u32 stack_ptr = (u32)&str;
	u32 offset = 4;
	while (*str) {
		if (*str == '%') {
			i8 next = *(str + 1);
			switch (next) {
			case '%':
				write_char('%');
				break;
			case 'b':
			{
				u32 v = *(u32*)(stack_ptr + offset);
				write_bin(v);
			}
			case 'd':
			{
				u32 v = *(u32*)(stack_ptr + offset);
				write_dec(v);
				offset += 4;
				break;
			}
			case 'x':
			{
				u32 v = *(u32*)(stack_ptr + offset);
				write_hex32(v);
				offset += 4;
				break;
			}
			case 's':
			{
				const i8* v = *(const i8**)(stack_ptr + offset);
				write_string(v);
				offset += 4;
				break;
			}
			case 'c':
			{
				u32 v = *(u32*)(stack_ptr + offset);
				write_char(v);
				offset += 4;
				break;
			}
			case 'C':
			{
				i8 next = *(str + 2);
				switch (next) {
				case 'f': /* VGA Foreground Color */
				{
					i8 c = *(str + 3);
					u8 v = hex2num(c);
					foreground_color = v;
				}
				case 'b':
				{
					i8 c = *(str + 3);
					u8 v = hex2num(c);
					background_color = v;
				}
				case 'r':
					reset_color();
					str++;
				default:
					assert(false, "printf: Invalid format string");
				}
			}
			default:
				assert(false, "printf: Invalid format string");
			}
			str++;
		} else {
			write_char(*str);
		}
		str++;
	}
}
