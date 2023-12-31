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
	   Enum of keys on a keyboard.
	 */
	enum class Key : u8 {
		Escape,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		PrintScreen,
		ScrollLock,
		Pause,
		BackTick,
		One,
		Two,
		Three,
		Four,
		Five,
		Six,
		Seven,
		Eight,
		Nine,
		Zero,
		Minus,
		Equal,
		BackSpace,
		Tab,
		Q,
		W,
		E,
		R,
		T,
		Y,
		U,
		I,
		O,
		P,
		LeftBracket,
		RightBracket,
		BackSlash,
		CapsLock,
		A,
		S,
		D,
		F,
		G,
		H,
		J,
		K,
		L,
		SemiColon,
		Apostrophe,
		Enter,
		LeftShift,
		Z,
		X,
		C,
		V,
		B,
		N,
		M,
		Comma,
		Period,
		Slash,
		RightShift,
		LeftControl,
		LeftSuper,
		LeftAlt,
		Space,
		RightAlt,
		Fn,
		RightSuper,
		RightControl,
		Insert,
		Home,
		PageUp,
		Delete,
		End,
		PageDown,
		Up,
		Left,
		Down,
		Right,
		NumLock,
		NumSlash,
		NumAsterisk,
		NumMinus,
		Num7,
		Num8,
		Num9,
		Num4,
		Num5,
		Num6,
		NumPlus,
		Num1,
		Num2,
		Num3,
		Num0,
		NumPeriod,
		NumEnter,
		WWWSearch,
		PreviousTrack,
		WWWFavorites,
		WWWRefresh,
		VolumeDown,
		Mute,
		WWWStop,
		Calculator,
		Apps,
		WWWForward,
		VolumeUp,
		PlayPause,
		Power,
		WWWBack,
		WWWHome,
		Stop,
		Sleep,
		MyComputer,
		Email,
		NextTrack,
		MediaSelect,
		Wake,

	};

	/**
	   Initialize the keyboard
	 */
	void init();

	/**
	   Enable scanning keys
	 */
	void enableScanning();

	/**
	   Disable scanning keys
	 */
	void disableScanning();

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
	void scan();

	/**
	   Check if a key is currently pressed

	   @param key is the key to check
	 */
	bool keyPressed(Key key);

	/**
	   I forgor
	 */
	void test();

	/**
	   Set if a key is pressed

	   @param key is the key to set
	   @param enabled is whether it should be enabled
	 */
	void setKey(Key key, bool enabled);
}
