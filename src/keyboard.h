#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "common.h"

/**
   The states of those lights on the keyboard. QEMU doesn't support
   forwarding these, so I don't ever use this
 */
enum KeyboardLED {
	SCROLL_LOCK = 1,
	NUMBER_LOCK = 2,
	CAPS_LOCK = 4,
};

/**
   Initialize the keyboard.
 */
void keyboard_init();

/**
   Enable keyboard scanning
 */
bool keyboard_enable_scanning();

/**
   Disable keyboard scanning
 */
bool keyboard_disable_scanning();

/**
   Set the LEDs of the keyboard
 */
bool keyboard_set_led(enum KeyboardLED);

/**
   Make sure the keyboard is working.
 */
bool keyboard_echo();

/**
   Set the keyboard scancode set. Don't use this. It doesn't work, and
   the code only supports set 1 anyways
 */
void keyboard_set_scancode_set(i8);

/**
   Get the keyboard scancode set.
 */
i8 keyboard_get_scancode_set();

/**
   Scan a key, and return the scanned key, with a weird format that I
   came up with.
 */
i8 keyboard_scan();

/**
   Translate my weird keycodes to chars.
 */
i8 translate_code(i8);

#endif
