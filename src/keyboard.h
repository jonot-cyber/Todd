#pragma once

#include "common.h"

namespace Keyboard {
	/**
	   Enum of LED states
	*/
	enum class LEDState : u8 {
		ScrollLock = 1,
		NumberLock = 2,
		CapsLock = 4,
	};

	/**
	   Initialize the keyboard
	 */
	void init();

	/**
	   Enable scanning keys
	 */
	bool enableScanning();

	/**
	   Disable scanning keys
	 */
	bool disableScanning();

	/**
	   Set the states of the LEDs

	   @param state is the state.
	 */
	bool setLed(LEDState state);

	/**
	   Make sure that the keyboard is working nice :)
	 */
	bool echo();

	/**
	   Set the scan code set

	   @param code is the scan code set #
	 */
	void setScanCodeSet(i8 code);

	/**
	   Get the scan code set
	 */
	i8 getScanCodeSet();

	/**
	   Scan from the keyboard
	 */
	i8 scan();

	/**
	   Check if a key is currently pressed

	   @param key is the key to check
	 */
	bool keyPressed(u8 key);

	/**
	   Set if a key is pressed

	   @param key is the key to set
	   @param enabled is whether it should be enabled
	 */
	void setKey(u8 key, bool enabled);

	i8 translateCode(i8 in);
}
