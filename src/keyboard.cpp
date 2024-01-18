#include "keyboard.h"

#include "ps2.h"

const i8 LSHIFT_CODE = 'F';
const i8 RSHIFT_CODE = 'G';

const i8 scanSet[256] = {
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

static bool keys[256]{};
static bool isScanning = false;

bool Keyboard::enableScanning() {
	if (isScanning) {
		return false;
	}
	isScanning = true;
	PS2::out(0xF4);
	return 0xFA == PS2::in();
}

bool Keyboard::disableScanning() {
	if (!isScanning) {
		return false;
	}
	isScanning = false;
	PS2::out(0xF5);
	return 0xFA == PS2::in();
}

void Keyboard::init() {
	disableScanning();
	for (bool& key : keys) {
		key = false;
	}
}

bool Keyboard::setLed(LEDState state) {
	disableScanning();
	PS2::out(0xED);
	PS2::out(static_cast<u8>(state));
	return PS2::in() == 0xFA;
}

bool Keyboard::echo() {
	disableScanning();
	PS2::out(0xEE);
	return PS2::in() == 0xEE;
}

void Keyboard::setScanCodeSet(i8 code) {
	disableScanning();
	PS2::out(0xF0);
	PS2::out(code);	
	return;      
}

i8 Keyboard::getScanCodeSet() {
	disableScanning();
	PS2::out(0xF0);
	PS2::out(0);
	u8 ret = PS2::in();
	if (ret != 0xFA) {
		return -1;
	}
	switch (PS2::in()) {
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

void Keyboard::setKey(u8 key, bool enabled) {
	keys[key] = enabled;
}

bool Keyboard::keyPressed(u8 key) {
	return keys[key];
}

i8 Keyboard::scan() {
	enableScanning();
	u8 ret = PS2::in();
	if (ret >= 0x80) {
		// Released
		i8 k = scanSet[ret - 0x80];
		if (!keyPressed(k)) {
			return '\0';
		}
		setKey(k, false);
		return -k;
	} else {
		i8 k = scanSet[ret];
		if (keyPressed(k)) {
			return '\0';
		}
		setKey(k, true);
		return k;
	}
	return '\0';
}

i8 Keyboard::translateCode(i8 in) {
	if (!(keyPressed(LSHIFT_CODE) || keyPressed(RSHIFT_CODE))) {
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
