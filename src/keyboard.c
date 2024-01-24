#include "keyboard.h"

#include "ps2.h"

const i8 LSHIFT_CODE = 'F';
const i8 RSHIFT_CODE = 'G';

const i8 SCAN_SET[256] = {
	'\0',
	'A',
	'1',
	'2',
	'3',
	'4',
	'5',
	'6',
	'7',
	'8',
	'9',
	'0',
	'-',
	'=',
	'\b',
	'\t',
	'q',
	'w',
	'e',
	'r',
	't',
	'y',
	'u',
	'i',
	'o',
	'p',
	'[',
	']',
	'\n',
	'E',
	'a',
	's',
	'd',
	'f',
	'g',
	'h',
	'j',
	'k',
	'l',
	';',
	'\'',
	'`',
	'F',
	'\\',
	'z',
	'x',
	'c',
	'v',
	'b',
	'n',
	'm',
	',',
	'.',
	'/',
	'G',
	'*',
	'H',
	' ',
	'J',
	'K',
	'L',
	'M',
	'N',
	'O',
	'P',
	'Q',
	'R',
	'T',
	'U',
	'V',
	'W',
	'7',
	'8',
	'9',
	'-',
	'4',
	'5',
	'6',
	'+',
	'1',
	'2',
	'3',
	'0',
	'.',
	'X',
	'Y',
};

static bool keys[256];
static bool is_scanning = false;

bool keyboard_enable_scanning() {
	if (is_scanning) {
		return false;
	}
	is_scanning = true;
	ps2_out(0xF4);
	return 0xFA == ps2_in();
}

bool keyboard_disable_scanning() {
	if (!is_scanning) {
		return false;
	}
	is_scanning = false;
	ps2_out(0xF5);
	return 0xFA == ps2_in();
}

void keyboard_init() {
	keyboard_disable_scanning();
	for (u32 i = 0; i < 256; i++) {
		keys[i] = false;
	}
}

bool keyboard_set_led(enum KeyboardLED state) {
	keyboard_disable_scanning();
	ps2_out(0xED);
	ps2_out((u8)state);
	return ps2_in() == 0xFA;
}

bool keyboard_echo() {
	keyboard_disable_scanning();
	ps2_out(0xEE);
	return ps2_in() == 0xEE;
}

void keyboard_set_scancode_set(i8 code) {
	keyboard_disable_scanning();
	ps2_out(0xF0);
	ps2_out(code);	
	return;      
}

i8 keyboard_get_scancode_set() {
	keyboard_disable_scanning();
	ps2_out(0xF0);
	ps2_out(0);
	u8 ret = ps2_in();
	if (ret != 0xFA) {
		return -1;
	}
	switch (ps2_in()) {
        case 0x43:
		return 1;
        case 0x41:
		return 2;
        case 0x3f:
		return 3;
        default:
		return -2;
	}
}

i8 keyboard_scan() {
	keyboard_enable_scanning();
	u8 ret = ps2_in();
	if (ret >= 0x80) {
		// Released
		i8 k = SCAN_SET[ret - 0x80];
		if (!keys[k]) {
			return '\0';
		}
		keys[k] = false;
		return -k;
	} else {
		i8 k = SCAN_SET[ret];
		if (keys[k]) {
			return '\0';
		}
		keys[k] = true;
		return k;
	}
	return '\0';
}

i8 translate_code(i8 in) {
	if (!(keys[LSHIFT_CODE] || keys[RSHIFT_CODE])) {
		return in;
	}
	if (in >= 'a' && in <= 'z') {
		return in - 'a' + 'A';
	}
	switch (in) {
	case '`':
		return '~';
	case '1':
		return '!';
	case '2':
		return '@';
	case '3':
		return '#';
	case '4':
		return '$';
	case '5':
		return '%';
	case '6':
		return '^';
	case '7':
		return '&';
	case '8':
		return '*';
	case '9':
		return '(';
	case '0':
		return ')';
	case '-':
		return '_';
	case '=':
		return '+';
	case '[':
		return '{';
	case ']':
		return '}';
	case '\\':
		return '|';
	case ';':
		return ':';
	case '\'':
		return '"';
	case ',':
		return '<';
	case '.':
		return '>';
	case '/':
		return '?';
	default:
		return in;
	}
}
