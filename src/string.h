#pragma once

#include "common.h"

u32 strlen(const i8* src);

/**
   Dynamically sized string
*/
namespace Types {
	struct String {
		i8* data;
		u32 len;

		String();
		String(u32 len);
		String(const i8* src);
		~String();
		String(const String& other);

		// Concatenate two strings together;
		String operator+(String& s);
		String operator+(const i8* s);
		String operator+(i8 c);
		String& operator+=(String& s);
		String& operator+=(const i8* s);
		String& operator+=(i8 c);

		void clear();

		i32 toInt();
		u32 toUint();
		static i32 toInt(const i8* s);
		static u32 toUint(const i8* s);
		static String toString(u32 v);
		static String toString(i32 v);
	};
};
