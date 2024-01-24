#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "common.h"

enum KeyboardLED {
	SCROLL_LOCK = 1,
	NUMBER_LOCK = 2,
	CAPS_LOCK = 4,
};

void keyboard_init();
bool keyboard_enable_scanning();
bool keyboard_disable_scanning();
bool keyboard_set_led(enum KeyboardLED led);
bool keyboard_echo();
void keyboard_set_scancode_set(i8 code);
i8 keyboard_get_scancode_set();
i8 keyboard_scan();
bool key_pressed(u8 key);
void set_key(u8 key, bool enabled);
i8 translate_code(i8 in);

#endif
