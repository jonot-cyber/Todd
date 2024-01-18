#include "string.h"

#include "memory.h"
#include "monitor.h"

u32 strlen(const i8* src) {
	assert(src != nullptr, "strlen: src is null");
	const i8* ptr = src;
	while (*ptr != '\0') {
		ptr++;
	}
	return ptr - src;
}

u32 strcmp(const i8* src1, const i8* src2) {
	for (u32 i = 0; i < 1000; i++) {
		if (src1[i] == '\0' && src2[i] == '\0') {
			return 0;
		}
		if (src1[i] == src2[i]) {
			continue;
		}
		if (src1[i] < src2[i]) {
			return -1;
		}
		return 1;
	}
	assert(false, "strcmp: Maxium string length exceeded");
}

u32 strcmpSpan(const i8* src, const i8* start, u32 size) {
	for (u32 i = 0; i < size; i++) {
		if (src[i] == '\0') {
			return -1;
		}
		if (src[i] == start[i]) {
			continue;
		}
		if (src[i] < start[i]) {
			return -1;
		}
		return 1;
	}
	return 0;
}

Types::String::String() {
	data = Memory::kmalloc<i8>();
	data[0] = '\0';
	len = 0;
}

Types::String::String(u32 length) {
	data = nullptr;
	len = length;
}

Types::String::String(const i8* src) {
	assert(src != nullptr, "Types::String::String: src is null");
	len = strlen(src);
	data = Memory::kmalloc<i8>(len+1);
	memcpy(src, data, len);
	data[len] = '\0';
}

// If you remove this, the compiler complains. IDK
void* _Unwind_Resume=0;
void* __gxx_personality_v0=0;

Types::String::~String() {
	Memory::kfree(data);
}

Types::String::String(const Types::String& other) {
	len = other.len;
	data = Memory::kmalloc<i8>(len+1);
	memcpy(other.data, data, len);
}

Types::String Types::String::operator+(Types::String& s) {
	Types::String ret(len + s.len);
	ret.data = Memory::kmalloc<i8>(ret.len + 1);
	memcpy(data, ret.data, len);
	memcpy(s.data, ret.data + len, s.len);
	ret.data[ret.len] = '\0';
	return ret;
}

Types::String Types::String::operator+(const i8* s) {
	u32 sLen = strlen(s);
	Types::String ret(len + sLen);
	ret.data = Memory::kmalloc<i8>(ret.len + 1);
	memcpy(data, ret.data, len);
	memcpy(s, ret.data+len, sLen);
	ret.data[ret.len] = '\0';
	return ret;
}

Types::String Types::String::operator+(i8 c) {
	Types::String ret(len + 1);
	ret.data = Memory::kmalloc<i8>(ret.len + 1);
	memcpy(data, ret.data, len);
	ret.data[len] = c;
	ret.data[ret.len] = '\0';
	return ret;
}

Types::String& Types::String::operator+=(Types::String& s) {
	u32 oldLen = len;
	len += s.len;
	i8* newData = Memory::kmalloc<i8>(len + 1);
	memcpy(data, newData, oldLen);
	memcpy(s.data, newData+len, s.len);
	newData[len] = '\0';
	Memory::kfree(data);
	data = newData;
	return *this;
}

Types::String& Types::String::operator+=(const i8* s) {
	u32 sLen = strlen(s);
	i8* newData = Memory::kmalloc<i8>(len + sLen + 1);
	memcpy(data, newData, len);
	memcpy(s, newData+len, sLen);
	Memory::kfree(data);
	data = newData;
	len += sLen;
	data[len] = '\0';
	return *this;
}

Types::String& Types::String::operator+=(i8 c) {
	i8* newData = Memory::kmalloc<i8>(len + 2);
	memcpy(data, newData, len);
	Memory::kfree(data);
	data = newData;
	data[len] = c;
	data[len+1] = '\0';
	len++;
	return *this;
}

void Types::String::clear() {
	Memory::kfree(data);
	data = Memory::kmalloc<i8>();
	*data = '\0';
	len = 1;
}


i32 Types::String::toInt() {
	bool isNegative = false;
	i32 ret = 0;
	
	u32 i = 0;
	if (data[0] == '-') {
		isNegative = true;
		i++;
		
	}
	for (; i < len; i++) {
		u8 c = data[i] - '0';
		if (c > 9) {
			Monitor::writeString("PANIC: Cannot convert string to integer\n");
			halt();
		}
		ret *= 10;
		ret += c;
	}
	if (isNegative) {
		return -ret;
	}
	return ret;
}

u32 Types::String::toUint() {
	i32 ret = 0;	
	for (u32 i = 0; i < len; i++) {
		u8 c = data[i] - '0';
		if (c > 9) {
			Monitor::writeString("PANIC: Cannot convert string to integer\n");
			halt();
		}
		ret *= 10;
		ret += c;
	}
	return ret;
}

i32 Types::String::toInt(const i8* s) {
	bool isNegative = false;
	i32 ret = 0;
	
	u32 i = 0;
	if (s[0] == '-') {
		isNegative = true;
		i++;
		
	}
	u32 len = strlen(s);
	for (; i < len; i++) {
		u8 c = s[i] - '0';
		if (c > 9) {
			Monitor::writeString("PANIC: Cannot convert string to integer\n");
			halt();
		}
		ret *= 10;
		ret += c;
	}
	if (isNegative) {
		return -ret;
	}
	return ret;
}

u32 Types::String::toUint(const i8* s) {
	i32 ret = 0;
	u32 len = strlen(s);
	for (u32 i = 0; i < len; i++) {
		u8 c = s[i] - '0';
		if (c > 9) {
			Monitor::writeString("PANIC: Cannot convert string to integer\n");
			halt();
		}
		ret *= 10;
		ret += c;
	}
	return ret;
}

Types::String Types::String::toString(u32 v) {
	u32 oldV = v;
	u32 digits = 0;
	while (oldV != 0) {
		oldV /= 10;
		digits++;
	}
	Types::String ret(digits);
	ret.data = Memory::kmalloc<i8>(digits + 1);
	for (u32 i = 0; i < digits; i++) {
		u8 digit = v % 10;
		v /= 10;
		ret.data[ret.len-i-1] = digit + '0';
	}
	ret.data[ret.len] = '\0';
	return ret;
}

Types::String Types::String::toString(i32 v) {
	bool isNegative = false;
	if (v < 0) {
		isNegative = true;
		v = -v;
	}
	u32 oldV = v;
	u32 digits = 0;
	while (oldV != 0) {
		oldV /= 10;
		digits++;
	}
	Types::String ret(digits + isNegative);
	ret.data = Memory::kmalloc<i8>(digits + isNegative + 1);
	if (isNegative) {
		ret.data[0] = '-';
	}
	for (u32 i = 0; i < digits; i++) {
		u8 digit = v % 10;
		v /= 10;
		ret.data[ret.len-i-1] = digit + '0';
	}
	ret.data[ret.len] = '\0';
	return ret;
}
